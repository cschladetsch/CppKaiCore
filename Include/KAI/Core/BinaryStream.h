#pragma once

#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Type/Properties.h>
#include <KAI/Core/Type/TraitMacros.h>

#include <vector>

#include "KAI/Core/BinaryPacket.h"
#include "KAI/Core/StringStream.h"

KAI_BEGIN

// A BinaryStream is-a BinaryPacket which can also resize and allows insertion
class BinaryStream : public BinaryPacket {
   public:
       using Byte = char;
       using Bytes = std::vector<Byte>;

   private:
       Bytes bytes_;

   public:
       BinaryStream() = default;
       BinaryStream(Registry& r) : BinaryPacket(r) {}
       BinaryStream(int startSize) : bytes_(startSize) {}

       BinaryStream& Write(int len, const Byte* src);
       void Clear();

       template <class POD> BinaryStream& Write(const POD& pod)
       {
           return Write(sizeof(pod), reinterpret_cast<const Byte*>(&pod));
       }

       friend bool operator<(const BinaryPacket& a, const BinaryPacket& b);
       friend bool operator==(const BinaryPacket& a, const BinaryPacket& b);

       static void Register(Registry&);
};

StringStream &operator<<(StringStream &, BinaryStream const &);
BinaryStream &operator<<(BinaryStream &, BinaryStream const &);
BinaryPacket &operator>>(BinaryPacket &, BinaryStream &);

KAI_TYPE_TRAITS(BinaryStream, Number::BinaryStream,
                Properties::StringStreamInsert |
                    Properties::BinaryStreamExtract |
                    Properties::BinaryStreamInsert)

KAI_END
