#pragma once

#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Object/Object.h>

KAI_BEGIN

/// a pair of objects
struct Pair {
    Object first, second;

    Pair() = default;
    Pair(const Object& a, const Object& b) : first(a), second(b) {}
    Pair(std::pair<Object, Object> const& p) : first(p.first), second(p.second) {}

    static void Register(Registry &);

    friend bool operator<(Pair const& a, Pair const& b)
    {
        // you may think this could be simpler
        return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
    }

    friend bool operator==(Pair const& a, Pair const& b)
    {
        return a.first == b.first && a.second == b.second;
    }
};

StringStream& operator<<(StringStream& s, Pair const&);
StringStream& operator>>(StringStream& s, Pair&);
BinaryStream& operator<<(BinaryStream& s, Pair const&);
BinaryPacket& operator>>(BinaryPacket& s, Pair&);

HashValue GetHash(const Pair &);

KAI_TYPE_TRAITS(Pair, Number::Pair,
                Properties::Streaming | Properties::Equiv | Properties::Assign |
                    Properties::Less);

KAI_END
