#include "KAI/Language/Common/LexerBase.h"

KAI_BEGIN

int IsSpaceChar(int ch) { return ch == ' '; }

LexerBase::LexerBase(const char *in, Registry &r)
    : ProcessCommon(r), input_(in), offset_(0), lineNumber_(0) {}

void LexerBase::CreateLines() {
    if (input_.back() != '\n') input_.push_back('\n');

    size_t lineStart = 0;
    for (size_t n = 0; n < input_.size(); ++n) {
        if (input_[n] == '\n') {
            lines_.push_back(input_.substr(lineStart, n - lineStart + 1));
            lineStart = n + 1;
        }
    }
}

char LexerBase::Current() const {
    if (lineNumber_ == (int)lines_.size()) return 0;

    return Line()[offset_];
}

const std::string &LexerBase::Line() const { return GetLine(lineNumber_); }

bool LexerBase::EndOfLine() const {
    auto len = (int)Line().size();
    return len == 0 || offset_ == (int)Line().size() - 1;
}

char LexerBase::Peek() const {
    if (!Current()) return 0;

    if (EndOfLine()) return 0;

    return Line()[offset_ + 1];
}

Slice LexerBase::Gather(int (*filt)(int)) {
    int start = offset_;
    while (filt(Next()));

    return Slice(start, offset_);
}

char LexerBase::Next() {
    if (EndOfLine()) {
        offset_ = 0;
        ++lineNumber_;
    } else
        ++offset_;

    if (lineNumber_ == (int)lines_.size()) return 0;

    return Line()[offset_];
}

bool LexerBase::LexString() {
    int start = offset_;
    Next();
    while (!failed && Current() != '"') {
        if (Current() == '\\') {
            switch (Next()) {
                case '"':
                case 'n':
                case 't':
                    break;

                default:
                    LexErrorBase("Bad escape sequence %c");
                    return false;
            }
        }

        if (Peek() == 0) {
            Fail("Bad string literal");
            return false;
        }

        Next();
    }

    Next();

    AddStringToken(lineNumber_, Slice(start + 1, offset_ - 1));

    return true;
}

bool LexerBase::LexShellCommand() {
#ifdef ENABLE_SHELL_SYNTAX
    int start = offset;
    Next();

    while (!Failed && Current() != '`') {
        if (Current() == 0) {
            Fail("Unterminated shell command");
            return false;
        }

        if (Current() == '\\') {
            char nextChar = Peek();
            if (nextChar == '`' || nextChar == '\\') {
                Next();
                if (Current() != 0) {
                    Next();
                }
            } else {
                Next();
            }
        } else {
            Next();
        }
    }

    if (Current() == '`') {
        Next();
    }

    AddShellCommandToken(lineNumber, Slice(start + 1, offset - 1));

    return true;
#else
    Fail(
        "Shell syntax (backtick operations) is disabled for security. Enable "
        "with -DENABLE_SHELL_SYNTAX=ON");
    return false;
#endif
}

KAI_END
