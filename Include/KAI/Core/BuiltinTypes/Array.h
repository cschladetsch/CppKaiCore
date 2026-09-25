#pragma once

#include <KAI/Core/Config/Base.h>

#include "Container.h"

KAI_BEGIN

class Array : public Container<Array> {
   public:
       using Objects = std::vector<Object>;
       using const_iterator = Objects::const_iterator;
       using iterator = Objects::iterator;

   private:
       Objects objects_;

   public:
       iterator Begin()
       {
           return objects_.begin();
       }
       iterator End()
       {
           return objects_.end();
       }
       [[nodiscard]] const_iterator Begin() const
       {
           return objects_.begin();
       }
       [[nodiscard]] const_iterator End() const
       {
           return objects_.end();
       }


    void Resize(int n) {
        objects_.resize(n);
    }
    [[nodiscard]] Object At(int pos) const
    {
        return objects_.at(pos);
    }
    Object &RefAt(int pos) {
        return objects_.at(pos);
    }
    [[nodiscard]] int Size() const
    {
        return static_cast<int>(objects_.size());
    }
    [[nodiscard]] bool Empty() const
    {
        return objects_.empty();
    }
    [[nodiscard]] Object Front() const
    {
        return objects_.front();
    }
    [[nodiscard]] Object Back() const
    {
        return objects_.back();
    }

    void Clear();
    void Append(Object const &);
    void PushBack(Object const& q)
    {
        Append(q);
    }
    Object PopBack();
    void RemoveAt(int);
    void Insert(int index, Object const &);

    iterator Erase(iterator a);
    void Erase(Handle a);
    void Erase(Object const& q);

    void Erase2(const Object& q)
    {
        Erase(q);
    }
    void Append2(const Object& q)
    {
        Append(q);
    }
    void Insert2(int index, const Object& q)
    {
        Insert(index, q);
    }

    friend bool operator==(const Array& a, const Array& b)
    {
        return a.objects_ == b.objects_;
    }
    friend bool operator<(const Array& a, const Array& b)
    {
        return a.objects_ < b.objects_;
    }
    friend Array operator+(const Array& a, const Array& b)
    {
        Array result;
        result.objects_ = a.objects_;
        result.objects_.insert(result.objects_.end(), b.objects_.begin(), b.objects_.end());
        return result;
    }
    friend Array operator+(const Array& a, const Object& b)
    {
        Array result;
        result.objects_ = a.objects_;
        result.objects_.push_back(b);
        return result;
    }

    // void SetChildSwitch(int N, bool M)
    //{
    //     ForEach(objects, SetSwitch<Array>(N, M));
    // }

    static void Register(Registry &);
};

inline Array::iterator begin(Array &a) { return a.Begin(); }
inline Array::iterator end(Array &a) { return a.End(); }
inline Array::const_iterator begin(Array const &a) { return a.Begin(); }
inline Array::const_iterator end(Array const &a) { return a.End(); }

StringStream &operator<<(StringStream &, const Array &);
BinaryStream &operator<<(BinaryStream &, const Array &);
BinaryStream &operator>>(BinaryStream &, Array &);

HashValue GetHash(const Array& a);

KAI_TYPE_TRAITS(Array, Number::Array,
                Properties::StringStreamInsert | Properties::BinaryStreaming |
                    Properties::Less | Properties::Equiv | Properties::Assign |
                    Properties::Reflected | Properties::Container |
                    Properties::Plus);

KAI_END
