#pragma once

#include <KAI/Language/Common/LexerBase.h>
#include <KAI/Language/Common/KaiProcess.h>
#include <KAI/Language/Common/Slice.h>

#include <algorithm>
#include <format>
#include <map>
#include <sstream>
#include <vector>

#ifdef KAI_USE_MONOTONIC_ALLOCATOR
#include <memory_resource>
#endif

KAI_BEGIN

template <class EnumType>
class LexerCommon : public LexerBase {
   public:
    typedef typename EnumType::Type Token;
    typedef typename EnumType::Enum Enum;
    typedef EnumType TokenEnumType;

#ifdef KAI_USE_MONOTONIC_ALLOCATOR
    // Arena (monotonic) allocation via the standard library's PMR facilities,
    // backed by arena_ (declared below). This replaces the former
    // boost.monotonic containers, so KAI carries no Boost dependency.
    typedef std::pmr::vector<Token> Tokens;
    typedef std::pmr::map<std::string, Enum> Keywords;
#else
    typedef std::vector<Token> Tokens;
    typedef std::map<std::string, Enum> Keywords;
#endif

    LexerCommon(const char *input, Registry &r) : LexerBase(input, r) {}

    bool Process() {
        AddKeyWords();
        CreateLines();
        return Run();
    }

    virtual void AddKeyWords() = 0;
    virtual bool NextToken() = 0;
    virtual void Terminate() = 0;

    const Tokens &GetTokens() const { return tokens; }

   protected:
#ifdef KAI_USE_MONOTONIC_ALLOCATOR
    // The arena must outlive (be declared before) the containers it backs.
    std::pmr::monotonic_buffer_resource arena_;
    Tokens tokens{&arena_};
    Keywords keyWords{&arena_};
#else
    Tokens tokens;
    Keywords keyWords;
#endif
    using LexerBase::reg_;

    bool Run() {
        offset = 0;
        lineNumber = 0;

        while (!Failed && NextToken());

        Terminate();

        return !Failed;
    }

    Token LexAlpha() {
        auto isIdentChar = [](int ch) -> int {
            return isalnum(ch) || ch == '_';
        };

        Token tok(Enum::Ident, *this, lineNumber, Gather(isIdentChar));
        auto kw = keyWords.find(tok.Text());
        auto keyword = kw != keyWords.end();
        if (keyword) tok.type = kw->second;

        return tok;
    }

    void AddStringToken(int lineNumber, Slice slice) override {
        tokens.push_back(Token(Enum::String, *this, lineNumber, slice));
    }

    void AddShellCommandToken(int lineNumber, Slice slice) override {
        tokens.push_back(Token(Enum::ShellCommand, *this, lineNumber, slice));
    }

    void LexErrorBase(const char *msg) override { LexError(msg); }

    bool Add(Token const &tok) {
        tokens.push_back(tok);
        return true;
    }

    bool Add(Enum type, Slice slice) {
        tokens.push_back(Token(type, *this, lineNumber, slice));
        return true;
    }

    bool Add(Enum type, int len = 1) {
        Add(type, Slice(offset, offset + len));
        while (len--) Next();

        return true;
    }

    bool AddIfNext(char ch, Enum thentype, Enum elseType) {
        if (Peek() == ch) {
            Next();
            return Add(thentype, 2);
        }

        return Add(elseType, 1);
    }

    bool AddTwoCharOp(Enum ty) {
        Add(ty, 2);
        Next();

        return true;
    }

    bool AddThreeCharOp(Enum ty) {
        Add(ty, 3);
        Next();
        Next();

        return true;
    }

    bool LexError(const char *text) {
        return Fail(CreateErrorMessage(Token(static_cast<Enum>(0), *this,
                                             lineNumber, Slice(offset, offset)),
                                       text, Current()));
    }

   public:
    // Builds a "(line):[col]: message\n" diagnostic, optionally followed by
    // source-line context. `fmt` uses std::format placeholders ("{}", not
    // printf's "%s"/"%c"/"%d"); callers with no substitutions just pass a
    // plain message and no extra args.
    //
    // `fmt` is a plain std::string_view rather than a compile-time-checked
    // std::format_string, because some callers (e.g. LexError() below) only
    // learn the format text at runtime, so it can't satisfy format_string's
    // consteval requirement. That trades compile-time placeholder checking
    // for the ability to accept those call sites uniformly - std::vformat
    // still throws std::format_error at runtime on a malformed format
    // string or an out-of-range argument reference.
    template <typename... Args>
    static std::string CreateErrorMessage(Token tok, std::string_view fmt,
                                          Args &&...args) {
        // Extra arguments the format string doesn't reference are simply
        // unused (not an error) - LexError() below always passes Current()
        // even for messages with no placeholder at all.
        std::string message;
        if constexpr (sizeof...(Args) > 0) {
            // std::make_format_args wants lvalues (it stores references, not
            // values, in the arg-store it returns) - pass the named
            // parameter pack directly rather than std::forward-ing it, since
            // forwarding would turn a by-value arg back into an rvalue that
            // can't bind to make_format_args' `Args&...` parameters.
            message = std::vformat(fmt, std::make_format_args(args...));
        } else {
            message = std::string(fmt);
        }

        std::string buff = std::format("({}):[{}]: {}\n", tok.lineNumber,
                                       tok.slice.Start, message);

        // tok.lexer is null for a default-constructed/sentinel Token - e.g.
        // ParserCommon's endToken_, returned by Current()/Consume()/etc. once
        // parsing has run past the end of the token stream on malformed
        // input. There's no source line to show for a token that doesn't
        // belong to any lexer, so return the message we have instead of
        // dereferencing a null pointer (this used to crash with an access
        // violation once the vector-subscript-out-of-range bug that used to
        // mask it was fixed).
        if (tok.lexer == nullptr) {
            return buff;
        }

        int beforeContext = 2;
        int afterContext = 2;

        const LexerBase &lex = *tok.lexer;
        int start = std::max(0, tok.lineNumber - beforeContext);
        int end = std::min((int)lex.GetLines().size() - 1,
                           tok.lineNumber + afterContext);

        std::stringstream err;
        err << buff << std::endl;
        for (int n = start; n <= end; ++n) {
            for (auto ch : lex.GetLine(n)) {
                if (ch == '\t')
                    err << "    ";
                else
                    err << ch;
            }

            if (n == tok.lineNumber) {
                for (int ch = 0; ch < (int)lex.GetLine(n).size(); ++ch) {
                    if (ch == tok.slice.Start) {
                        err << '^';
                        break;
                    }

                    auto c = lex.GetLine(tok.lineNumber)[ch];
                    if (c == '\t')
                        err << "    ";
                    else
                        err << ' ';
                }

                err << std::endl;
            }
        }

        return err.str();
    }

    std::string Print() const {
        std::stringstream str;
        for (const auto &tok : tokens) {
            str << tok << ", ";
        }
        return str.str();
    }
};

KAI_END
