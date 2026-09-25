
#include "KAI/Core/BinaryStream.h"

#include "KAI/Core/Object.h"

KAI_BEGIN

BinaryStream &operator<<(BinaryStream &S, const BinaryPacket &T) {
    // Length-prefixed; must mirror operator>>(BinaryPacket &, BinaryPacket &).
    const int size = T.Size();
    S.Write(size);
    if (size > 0) S.Write(size, T.Begin());
    return S;
}

BinaryStream &operator<<(BinaryStream &S, const BinaryStream &T) {
    // Length-prefixed; must mirror operator>>(BinaryPacket &, BinaryStream &).
    const int size = T.Size();
    S.Write(size);
    if (size > 0) S.Write(size, T.Begin());
    return S;
}

void BinaryStream::Register(Registry &registry) {
    ClassBuilder<BinaryStream>(registry,
                               Label(Type::Traits<BinaryStream>::Name()))
        .methods("Size", &BinaryStream::Size)("Clear", &BinaryStream::Clear);
}

KAI_END
