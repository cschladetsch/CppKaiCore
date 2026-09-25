#pragma once

#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Object/Object.h>
#include <KAI/Core/TriColor.h>

#include <unordered_set>

#include "Container.h"

KAI_BEGIN

struct CompareHandles {
    bool operator()(Object const& a, Object const& b) const
    {
        return a.GetHandle() == b.GetHandle();
    }
};

struct HashObject {
    size_t operator()(Object const& a) const
    {
        return a.GetHandle().GetValue();
    }
};

// I'm sure there was a good reason I named this ObjectSet rather than just Set.
// And at this point, I am too afraid to ask.
struct ObjectSet : Container<ObjectSet> {
    using Objects = std::unordered_set<Object, HashObject, CompareHandles>;
    using const_iterator = Objects::const_iterator;
    using iterator = Objects::iterator;

private:
    Objects objects_;

public:
    bool Destroy() override;

    [[nodiscard]] const_iterator Begin() const
    {
        return objects_.begin();
    }
    [[nodiscard]] const_iterator End() const
    {
        return objects_.end();
    }
    iterator Begin()
    {
        return objects_.begin();
    }
    iterator End()
    {
        return objects_.end();
    }

    [[nodiscard]] int Size() const;
    [[nodiscard]] bool Empty() const;
    void Clear();
    iterator Erase(iterator);
    iterator Erase(Object const &);
    void Append(Object const &);
    void Insert(Object const &);
    void Remove(Object const &);
    bool Contains(Object const &);

    void SetChildSwitch(int n, bool m)
    {
        ForEach(objects_, SetSwitch<ObjectSet>(n, m));
    }

    static void Register(Registry &);
};

inline ObjectSet::iterator begin(ObjectSet &s) { return s.Begin(); }
inline ObjectSet::iterator end(ObjectSet &s) { return s.End(); }
inline ObjectSet::const_iterator begin(ObjectSet const &s) { return s.Begin(); }
inline ObjectSet::const_iterator end(ObjectSet const &s) { return s.End(); }

StringStream &operator<<(StringStream &, ObjectSet const &);
StringStream &operator>>(StringStream &, ObjectSet &);

BinaryStream &operator<<(BinaryStream &, ObjectSet const &);
BinaryStream &operator>>(BinaryStream &, ObjectSet &);

KAI_TYPE_TRAITS(ObjectSet, Number::Set,
                Properties::StringStreamInsert | Properties::BinaryStreaming |
                    Properties::Reflected | Properties::Container);

KAI_END
