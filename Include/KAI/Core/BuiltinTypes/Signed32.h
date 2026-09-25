#pragma once

#include <KAI/Core/Type.h>

KAI_BEGIN

StringStream& operator<<(StringStream& s, int n);
StringStream& operator>>(StringStream& s, int& n);
BinaryStream& operator<<(BinaryStream& s, int n);
BinaryStream& operator>>(BinaryStream& s, int& n);

// inline HashValue GetHash(int N) { return N; }

KAI_TYPE_TRAITS(int, Number::Signed32,
                Properties::Arithmetic | Properties::Multiplicative |
                    Properties::Streaming | Properties::Assign |
                    Properties::Relational | Properties::Absolute |
                    Properties::Boolean);

// KAI_TYPE_TRAITS(String, Number::String
//     , Properties::Arithmetic
//     | Properties::Streaming
//     | Properties::Assign
//     | Properties::Relational
//     );
KAI_END
