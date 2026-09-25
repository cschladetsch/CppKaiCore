#pragma once

#include <KAI/Core/Config/Base.h>

#include "Registry.h"

KAI_BEGIN

/// A Binary-packet is a fixed-size sequence of bytes which allows only
/// extraction this allows it to be used to extract data from network packets
/// without copying.
class BinaryPacket {
   public:
       using Byte = char;
       using const_iterator = const Byte *;

   protected:
       const_iterator first_, current_, last_;
       Registry* registry_;

   public:
       BinaryPacket() : registry_(nullptr)
       {
           first_ = last_ = current_ = nullptr;
       }
       BinaryPacket(Registry& r) : registry_(&r)
       {
           first_ = last_ = current_ = nullptr;
       }
       BinaryPacket(const_iterator f, const_iterator l, Registry* r = nullptr)
           : first_(f), last_(l), current_(f), registry_(r)
       {
       }

       [[nodiscard]] const_iterator Begin() const
       {
           return first_;
       }
       [[nodiscard]] const_iterator Current() const
       {
           return current_;
       }
       [[nodiscard]] const_iterator Last() const
       {
           return last_;
       }

       [[nodiscard]] int Size() const
       {
           return static_cast<int>(last_ - first_);
       }
       [[nodiscard]] bool Empty() const
       {
           return last_ == first_;
       }
    bool Read(int len, Byte *dest);
    [[nodiscard]] bool CanRead(int len) const;

    template <class POD>
    bool Read(POD &pod) {
        return Read(sizeof(pod), reinterpret_cast<Byte *>(&pod));
    }

    void SetRegistry(Registry* r)
    {
        registry_ = r;
    }
    [[nodiscard]] Registry* GetRegistry() const
    {
        return registry_;
    }
void Reset() {
    current_ = first_;
}

    static void Register(Registry &, const char *);

    friend bool operator<(const BinaryPacket& a, const BinaryPacket& b);
    friend bool operator==(const BinaryPacket& a, const BinaryPacket& b);
};

StringStream &operator<<(StringStream &, BinaryPacket const &);
BinaryStream &operator<<(BinaryStream &, BinaryPacket const &);
BinaryPacket &operator>>(BinaryPacket &, BinaryPacket &);

KAI_TYPE_TRAITS(BinaryPacket, Number::BinaryPacket,
                Properties::StringStreamInsert |
                    Properties::BinaryStreamExtract |
                    Properties::BinaryStreamInsert)

KAI_END
