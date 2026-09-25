#pragma once

#include <KAI/Core/Object/Object.h>

#include <list>

#include "KAI/Core/BuiltinTypes/Container.h"

KAI_BEGIN

/// A list of Objects
struct List : Container<List> {
    using Objects = std::list<Object>;
    using const_iterator = Objects::const_iterator;
    using iterator = Objects::iterator;

private:
    Objects objects_;

public:
    bool Destroy() override
    {
        Clear();
        return true;
    }

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

    void Append(Object const& q);
    void PushBack(Object const& q)
    {
        Append(q);
    }
    Object Pop();
    Object PopBack() { return Pop(); }
    iterator Erase(iterator a);
    iterator Erase(Object const& q);
    [[nodiscard]] bool Contains(Object const& q) const
    {
        for (auto const& element : objects_) {
            if (element.GetHandle() == q.GetHandle()) {
                return true;
            }
        }
        return false;
    }

    void Erase2(const Object& q)
    {
        Erase(q);
    }
    void Append2(const Object& q)
    {
        Append(q);
    }
    [[nodiscard]] bool Contains2(const Object& q) const
    {
        return Contains(q);
    }

    friend bool operator==(const List& a, const List& b)
    {
        return a.objects_ == b.objects_;
    }
    friend bool operator<(const List& a, const List& b)
    {
        return a.objects_ < b.objects_;
    }

    void SetChildSwitch(int n, bool m)
    {
        for (auto& elem : objects_) {
            elem.SetSwitch(n, m);
        }
    }

    static void Register(Registry &);
};

inline List::iterator begin(List &l) { return l.Begin(); }
inline List::iterator end(List &l) { return l.End(); }
inline List::const_iterator begin(List const &l) { return l.Begin(); }
inline List::const_iterator end(List const &l) { return l.End(); }

StringStream &operator<<(StringStream &, const List &);
BinaryStream &operator<<(BinaryStream &, const List &);
BinaryStream &operator>>(BinaryStream &, List &);

HashValue GetHash(const List& a);

KAI_TYPE_TRAITS(List, Number::List,
                Properties::StringStreamInsert | Properties::BinaryStreaming |
                    Properties::Less | Properties::Equiv | Properties::Assign |
                    Properties::Reflected | Properties::Container);

KAI_END

// EOF
