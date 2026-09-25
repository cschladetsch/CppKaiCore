#pragma once

#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Meta/Base.h>
#include <KAI/Core/Type/Number.h>
#include <KAI/Core/Type/Traits.h>

KAI_BEGIN

struct Void {};

inline StringStream& operator<<(StringStream& s, Void const& /*unused*/)
{
    return s;
}
inline StringStream& operator>>(StringStream& s, Void& /*unused*/)
{
    return s;
}
inline BinaryStream& operator<<(BinaryStream& s, Void const& /*unused*/)
{
    return s;
}
inline BinaryPacket& operator>>(BinaryPacket& s, Void& /*unused*/)
{
    return s;
}

inline HashValue GetHash(Void /*unused*/)
{
    return 42;
}

namespace Type {
template <>
struct Traits<void>
    : TraitsBase<void, Number::Void, 0, /*TODO Properties::Streaming,*/ Void,
                 Void &, const Void &> {
    static const char *Name() { return "void"; }

    struct ContainerOps {
        template <class A, class B, class C> static void SetSwitch(A /*unused*/, B /*unused*/, C /*unused*/) {}

        template <class A, class B> static void SetMarked(A /*unused*/, B /*unused*/) {}

        template <class A, class B> static void ForEachContained(A /*unused*/, B /*unused*/) {}

        template <class A, class B> static void MakeReachableGrey(A /*unused*/, B /*unused*/) {}

        template <class A, class B> static void DetachFromContainer(A /*unused*/, B /*unused*/) {}

        template <class A, class B> static void Erase(A /*unused*/, B /*unused*/){};
    };
};

template <>
struct Traits<meta::Null> : Traits<void> {};
template <>
struct Traits<Void> : Traits<void> {};
}  // namespace Type

KAI_END
