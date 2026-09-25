#include "KAI/Core/BuiltinTypes.h"

KAI_BEGIN

void BinaryPacket::Register(Registry &registry, const char *name) {
    ClassBuilder<BinaryPacket>(registry, Label(name))
        .methods("Size", &BinaryPacket::Size);
}

bool BinaryPacket::Read(int len, Byte *dest) {
    if (!CanRead(len)) return false;

    memcpy(dest, current_, len);
    current_ += len;
    return true;
}

bool BinaryPacket::CanRead(int len) const {
    return len > 0 && current_ + len <= last_;
}

void BinaryStream::Clear() {
    bytes_.clear();
    first_ = current_ = last_ = 0;
}

BinaryStream &BinaryStream::Write(int len, const Byte *src) {
    if (len <= 0) {
        return *this;  // Nothing to write
    }

    // Calculate current_ state
    std::size_t cursor = current_ - first_;
    std::size_t cur_size = last_ - first_;

    // Reserve space efficiently - avoid frequent reallocations
    // If we need to grow, use exponential growth strategy
    size_t new_size = bytes_.size() + len;
    if (bytes_.capacity() < new_size) {
        bytes_.reserve(std::max(new_size, bytes_.size() * 2));
    }

    // Resize the buffer to fit the new data
    bytes_.resize(new_size);

    // Update pointers after resize
    first_ = bytes_.data();
    current_ = first_ + cursor;
    last_ = first_ + new_size;

    // Copy the new data at the end of current_ data
    memcpy((void *)(first_ + cur_size), src, len);

    return *this;
}

BinaryPacket &operator>>(BinaryPacket &S, BinaryPacket &T) {
    // Length-prefixed; must mirror operator<<(BinaryStream &, const BinaryPacket &).
    int size = 0;
    if (!S.Read(size) || size < 0 || (size > 0 && !S.CanRead(size))) KAI_THROW_0(PacketExtraction);
    const BinaryPacket::Byte *data = S.Current();
    T = BinaryPacket(data, data + size, S.GetRegistry());
    for (int i = 0; i < size; ++i) {
        BinaryPacket::Byte dummy;
        S.Read(dummy);
    }
    return S;
}

BinaryPacket &operator>>(BinaryPacket &S, BinaryStream &T) {
    // Length-prefixed; must mirror operator<<(BinaryStream &, const BinaryStream &).
    int size = 0;
    if (!S.Read(size) || size < 0 || (size > 0 && !S.CanRead(size))) KAI_THROW_0(PacketExtraction);
    T.Clear();
    if (size > 0) {
        std::vector<BinaryPacket::Byte> buffer(static_cast<std::size_t>(size));
        if (!S.Read(size, buffer.data())) KAI_THROW_0(PacketExtraction);
        T.Write(size, buffer.data());
    }
    return S;
}

KAI_END

// EOF
