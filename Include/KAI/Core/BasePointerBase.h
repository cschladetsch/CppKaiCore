#pragma once

#include <KAI/Core/Base.h>

KAI_BEGIN

struct BasePointerBase {
    virtual ~BasePointerBase() = default;

    void Create() {}
    static bool Destroy()
    {
        return true;
    }

    static void Register(Registry &);
};

StringStream &operator<<(StringStream &, const BasePointerBase &);
StringStream &operator>>(StringStream &, BasePointerBase &);
BinaryStream &operator<<(BinaryStream &, const BasePointerBase &);
BinaryStream &operator>>(BinaryStream &, BasePointerBase &);

KAI_END
