#pragma once

#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Pointer.h>

KAI_BEGIN

/// helper structure to switch on system and non-system types
template <class T>
struct TypeInfo {
    enum { IsSytem = false };  // is this a system type?
    using StorageType = T;     // the type stored in the parent structure
    using ValueType = T;       // the underlying value type
};

template <>
struct TypeInfo<Object> {
    enum { IsSytem = static_cast<int>(true) };
    using StorageType = Object;
    using ValueType = Object;
};

template <class T>
struct TypeInfo<Pointer<T> > {
    enum { IsSytem = true };
    using StorageType = Pointer<T>;
    using ValueType = T;
};

KAI_END
