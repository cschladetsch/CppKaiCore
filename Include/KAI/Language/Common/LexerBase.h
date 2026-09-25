#pragma once

#include <KAI/Language/Common/ProcessCommon.h>
#include <KAI/Language/Common/Slice.h>

KAI_BEGIN

int IsSpaceChar(int ch);

class LexerBase : public ProcessCommon {
   public:
       using Lines = std::vector<std::string>;

       LexerBase(const char*, Registry& r);

       const std::string& GetLine(size_t n) const
       {
           if (lines_.empty() || n >= lines_.size()) {
               KAI_THROW_2(OutOfBounds, n, 0);
           }
           return lines_[n];
       }

    const Lines &GetLines() const {
        return lines_;
    }
    const std::string &GetInput() const {
        return input_;
    }
    int GetOffset() const {
        return offset_;
    }
    int GetLineNumber() const {
        return offset_;
    }
    const std::string &Line() const;
    std::string GetString(Slice const &slice) const {
        int length = slice.Length();
        return length == 0 ? "" : input_.substr(slice.Start, length);
    }

   protected:
       Lines lines_;
       std::string input_;
       int offset_, lineNumber_;
       using ProcessCommon::reg_;

       void CreateLines();
       bool LexString();
       bool LexShellCommand();
       char Current() const;
       char Next();
       bool EndOfLine() const;
       char Peek() const;
       char PeekBase() const;

       virtual void LexErrorBase(const char* msg) = 0;
       virtual void AddStringToken(int lineNumber, Slice slice) = 0;
       virtual void AddShellCommandToken(int lineNumber, Slice slice) = 0;

       Slice Gather(int (*filter)(int ch));
};

KAI_END
