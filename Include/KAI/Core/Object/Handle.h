#pragma once

#include <KAI/Core/Config/Base.h>

#include <unordered_set>

KAI_BEGIN

class Handle {
   public:
       using Value = int;

   private:
       Value value_;

   public:
       explicit Handle(Value v = 0) : value_(v) {}

       [[nodiscard]] Value GetValue() const
       {
           return value_;
       }
    Value NextValue() {
        return ++value_;
    }

    friend bool operator<(Handle a, Handle b)
    {
        return a.value_ < b.value_;
    }
    friend bool operator==(Handle a, Handle b)
    {
        return a.value_ == b.value_;
    }
    friend bool operator!=(Handle a, Handle b)
    {
        return a.value_ != b.value_;
    }
    operator bool() const {
        return GetValue() != 0;
    }

    static void Register(Registry &);
};

struct HashHandle {
    enum { BucketSize = 8, MinBuckets = 2048 };
    std::size_t operator()(const Handle& a) const
    {
        return a.GetValue();
    }

    bool operator()(const Handle& a, const Handle& b) const
    {
        return a.GetValue() < b.GetValue();
    }
};

StringStream &operator<<(StringStream &, Handle);

using HandleSet = std::unordered_set<Handle, HashHandle>;

KAI_END
