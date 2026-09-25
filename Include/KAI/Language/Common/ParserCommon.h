#pragma once

#include <KAI/Language/Common/AstNodeBase.h>
#include <KAI/Language/Common/ParserBase.h>
#include <KAI/Language/Common/Process.h>
#include <KAI/Language/Common/ProcessCommon.h>
#include <KAI/Language/Common/Structure.h>

KAI_BEGIN

// common for all parsers.
// iterate over a stream of tokens to produce an abstract syntax tree_
template <class ELexer, class AstEnumStruct>
class ParserCommon : public ProcessCommon {
   public:
       using Lexer = ELexer;
       using TokenNode = typename Lexer::Token;
       using TokenEnumType = typename Lexer::TokenEnumType;
       using TokenEnum = typename TokenNode::Enum;
       using AstEnum = typename AstEnumStruct::Enum;
       using AstNode = AstNodeBase<TokenNode, AstEnumStruct>;
       using AstNodePtr = std::shared_ptr<AstNode>;

       explicit ParserCommon(Registry& r) : ProcessCommon(r), current(0), indent(0)
       {
           lexer_.reset();
       }

    virtual bool Process(std::shared_ptr<Lexer> lex, Structure st) = 0;

    const std::string &GetError() const {
        return error_;
    }
    AstNodePtr GetRoot() const {
        return root_;
    }
    bool Process();

    template <class T>
    Pointer<T> New(T const &val) {
        return reg_->New<T>(val);
    }

    bool Run(Structure st) {
        try {
            Process(st);
        } catch (exception::Base& e) {
            if (!failed) {
                Fail(Lexer::CreateErrorMessage(Current(), "%s", e.ToString()));
            }
        } catch (std::exception& f) {
            if (!failed) {
                Fail(Lexer::CreateErrorMessage(Current(), "%s", f.what()));
            }
        } catch (...) {
            if (!failed) {
                Fail(Lexer::CreateErrorMessage(Current(), "internal error"));
            }
        }

        return !failed;
    }

    std::string PrintTree() const {
        std::stringstream str;
        PrintTree(str, 0, root_);
        return str.str();
    }

    std::string ToString() const {
        return root_->ToString();
    }

   protected:
    void PrintTree(std::ostream &str, int level, AstNodePtr root) const {
        auto val = root->ToString();
        if (val.empty()) {
            return;
        }
        std::string indent(4 * level, ' ');
        str << indent << val.c_str() << std::endl;
        for (auto const &ch : root->GetChildren()) {
            PrintTree(str, level + 1, ch);
        }
    }

    std::vector<TokenNode> tokens_;
    std::vector<AstNodePtr> stack_;
    size_t current;
    AstNodePtr root_;
    std::string error_;
    int indent;
    std::shared_ptr<Lexer> lexer_;

    bool Has() const {
        return current < tokens_.size();
    }

    bool Push(AstNodePtr node) {
        if (node) {
            stack_.push_back(node);
            return true;
        }

        return false;
    }

    bool Append(Object q)
    {
        if (Empty()) {
            return false;
        }
        Top()->Children.push_back(std::make_shared<AstNode>(AstEnum::Object, q));
        return true;
    }

    AstNodePtr Pop() {
        if (stack_.empty()) {
            // MUST CreateError("Internal Error: Parse stack empty");
            KAI_THROW_0(EmptyStack);
        }

        auto last = stack_.back();
        stack_.pop_back();

        return last;
    }

    AstNodePtr Top() {
        if (stack_.empty()) { KAI_THROW_0(EmptyStack); }
        return stack_.back();
    }

    bool PushConsume() {
        Push(NewNode(Consume()));
        return true;
    }

    static TokenNode const &EndToken() { static const TokenNode kEnd; return kEnd; }

    TokenNode const &Next() {
        // First check if tokens vector is empty
        if (tokens_.empty()) {
            KAI_TRACE_ERROR_1(Fail("No tokens to process in Next()"));
        }

        // Increment token index
        ++current;

        // Check if the new index is valid
        if (current >= tokens_.size()) {
            KAI_TRACE_ERROR_1(Fail("Next token index out of range"));
        }

        return current < tokens_.size() ? tokens_[current] : EndToken();
    }

    TokenNode const &Last() {
        // Check if tokens vector is empty
        if (tokens_.empty()) {
            KAI_TRACE_ERROR_1(Fail("No tokens to process in Last()"));
        }

        // Check if we can access the previous token
        if (current <= 0) {
            KAI_TRACE_ERROR_1(Fail("No previous token available"));
        }

        return (current > 0 && current - 1 < tokens_.size()) ? tokens_[current - 1] : EndToken();
    }

    TokenNode const &Current() const {
        // First check if tokens vector is empty to avoid range check error
        if (tokens_.empty()) {
            KAI_TRACE_ERROR_1(Fail("No tokens to process"));
        }

        if (current >= tokens_.size()) {
            KAI_TRACE_ERROR_1(Fail("Token index out of range"));
        }

        return current < tokens_.size() ? tokens_[current] : EndToken();
    }

    bool Current(TokenNode node) const {
        if (current >= tokens_.size()) {
            return false;
        }

        return tokens_[current] == node;
    }

    bool Empty() const {
        return current >= tokens_.size();
    }

    TokenNode const &Peek() const {
        // Check if tokens vector is empty
        if (tokens_.empty()) {
            KAI_TRACE_ERROR_1(Fail("No tokens to process in Peek()"));
        }

        if (current + 1 >= tokens_.size()) {
            KAI_TRACE_ERROR() << "Unexpected end of tokens stream";
        }

        return current + 1 < tokens_.size() ? tokens_[current + 1] : EndToken();
    }

    bool PeekConsume(TokenEnum ty) {
        if (Peek().type == ty) {
            Consume();
            return true;
        }

        return false;
    }

    bool CurrentIs(TokenEnum ty) const { return Current().type == ty; }

    bool PeekIs(TokenEnum ty) const { return Peek().type == ty; }

    bool Consume(TokenEnum ty) {
        if (Current().type == ty) {
            Consume();
            return true;
        }

        return false;
    }

    TokenNode const &Consume() {
        if (current == tokens_.size()) {
            KAI_TRACE_ERROR_1(Fail("Unexpected end of file"));
        }

        return current < tokens_.size() ? tokens_[current++] : EndToken();
    }

    bool Try(std::vector<TokenEnum> const &types) {
        for (auto ty : types) {
            if (Current().type == ty) {
                return true;
            }
        }
        return false;
    }

    bool Try(TokenEnum type) {
        // Make sure there are tokens to examine and current index is in bounds
        if (tokens_.empty() || current >= tokens_.size()) {
            return false;  // No tokens or current out of bounds
        }
        return tokens_[current].type == type;
    }

    AstNodePtr Expect(TokenEnum type) {
        TokenNode tok = Current();
        if (tok.type != type) {
            Fail(Lexer::CreateErrorMessage(tok, "Expected %s, have %s",
                                           TokenEnumType::ToString(type),
                                           TokenEnumType::ToString(tok.type)));
            return nullptr;
        }

        auto consumed = Consume();
        return std::make_shared<AstNode>(consumed);
    }

    AstNodePtr NewNode(AstEnum t) { return std::make_shared<AstNode>(t); }
    AstNodePtr NewNode(AstEnum e, TokenNode const &t) const {
        return std::make_shared<AstNode>(e, t);
    }
    AstNodePtr NewNode(TokenNode const &t) {
        return std::make_shared<AstNode>(t);
    }
};

KAI_END
