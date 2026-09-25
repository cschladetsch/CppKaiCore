#pragma once

#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Type/Deref.h>
#include <KAI/Core/Type/Number.h>

#include "KAI/Core/Object/Object.h"
#include "KAI/Core/Object/Storage.h"
#include "KAI/Core/Object/StorageBase.h"

KAI_BEGIN

StorageBase& GetStorageBase(Object const& q);

Type::Number GetTypeNumber(Object const& q);

template <class T> Storage<typename Type::Traits<T>::Store>& GetStorage(Pointer<T> const& p)
{
    return GetStorage<T>(GetStorageBase(p));
}

template <class T>
Storage<typename Type::Traits<T>::Store> &GetStorage(StorageBase &base) {
    if (base.GetTypeNumber().GetValue() != Type::Traits<T>::Number) {
        KAI_THROW_2(TypeMismatch, base.GetTypeNumber().ToInt(), Type::Traits<T>::Number);
    }
    return static_cast<Storage<typename Type::Traits<T>::Store> &>(base);
}

template <class T> DerefType<T>::Reference Deref(StorageBase& base)
{
    if (base.IsConst()) {
        KAI_THROW_0(ConstError);
    }
    return GetStorage<typename DerefType<T>::Value>(base).GetReference();
}

template <class T> DerefType<T>::Reference CleanDeref(StorageBase& base)
{
    return GetStorage<typename DerefType<T>::Value>(base).GetCleanReference();
}

template <class T> DerefType<T>::Reference Deref(Object const& q)
{
    return Deref<T>(GetStorageBase(q));
}

template <class T>
ConstStorage<T> const &GetConstStorage(StorageBase const &base) {
    if (base.GetTypeNumber() != Type::Traits<T>::Number) {
        KAI_THROW_2(TypeMismatch, base.GetTypeNumber().ToInt(), Type::Traits<T>::Number);
    }
    return static_cast<ConstStorage<T> const &>(base);
}

template <class T> DerefType<T>::ConstReference ConstDeref(StorageBase const& base)
{
    return GetConstStorage<typename DerefType<T>::Value>(base)
        .GetConstReference();
}

template <class T> DerefType<T>::ConstReference ConstDeref(const Object& q)
{
    return ConstDeref<T>(GetStorageBase(q));
}

template <> inline Object& Deref<Object>(Object const& q)
{
    return const_cast<Object&>(q);
}

template <> inline const Object& ConstDeref<Object>(const Object& q)
{
    return q;
}

KAI_END
