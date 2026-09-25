#pragma once

#include <KAI/Core/Base.h>

KAI_BEGIN

struct StorageBase;
template <class T>
class Storage;

class ClassBase;
template <class T>
class Class;

class Object;
class String;
class StringStream;
class BinaryStream;
class BinaryPacket;
struct Registry;
class Reflected;

template <class T>
struct FwdBasePointer {};

const char *ToLower(const char *str);

using HashValue = ::std::size_t;

KAI_END

#include <KAI/Core/Object/Reflected.h>
