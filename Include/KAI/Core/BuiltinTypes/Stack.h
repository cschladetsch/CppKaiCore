
#pragma once

#include <KAI/Core/BuiltinTypes/Container.h>
#include <KAI/Core/Config/Base.h>

#include <vector>

#include "KAI/Core/Exception/ExceptionBase.h"
#include "KAI/Core/Exception/ExceptionMacros.h"

KAI_BEGIN

class Stack : public Container<Stack> {
   public:
       using Objects = std::vector<Object>;
       using const_reverse_iterator = Objects::const_reverse_iterator;
       using reverse_iterator = Objects::reverse_iterator;
       using const_iterator = Objects::const_iterator;
       using iterator = Objects::iterator;

   private:
       Objects stack_;

   public:
       friend bool operator==(const Stack& a, const Stack& b)
       {
           return a.stack_ == b.stack_;
       }
       friend bool operator<(const Stack& a, const Stack& b)
       {
           return a.stack_ < b.stack_;
       }

       bool Destroy() override;

       iterator Begin()
       {
           return stack_.begin();
       }
    iterator End() {
        return stack_.end();
    }
    [[nodiscard]] const_iterator Begin() const
    {
        return stack_.begin();
    }
    [[nodiscard]] const_iterator End() const
    {
        return stack_.end();
    }
    [[nodiscard]] bool Empty() const
    {
        return stack_.empty();
    }
    [[nodiscard]] int Size() const
    {
        return static_cast<int>(stack_.size());
    }

    [[nodiscard]] Object At(int n) const;
    void Push(Object const& q);
    void Clear();
    iterator Erase(Object const &);
    iterator Erase(iterator);
    Object Pop();
    [[nodiscard]] Object Top() const;

    static void Register(Registry &);

    [[nodiscard]] const Objects& GetStack() const
    {
        return stack_;
    }
};

inline Stack::iterator begin(Stack &s) { return s.Begin(); }
inline Stack::iterator end(Stack &s) { return s.End(); }
inline Stack::const_iterator begin(Stack const &s) { return s.Begin(); }
inline Stack::const_iterator end(Stack const &s) { return s.End(); }

StringStream &operator<<(StringStream &, const Stack &);
BinaryStream &operator<<(BinaryStream &, const Stack &);
BinaryStream &operator>>(BinaryStream &, Stack &);

HashValue GetHash(const Stack &);

KAI_TYPE_TRAITS(Stack, Number::Stack,
                Properties::StringStreamInsert | Properties::BinaryStreaming |
                    Properties::Less | Properties::Equiv | Properties::Assign |
                    Properties::Reflected | Properties::Container);

KAI_END
