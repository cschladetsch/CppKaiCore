#pragma once

#include <KAI/Core/Config/Base.h>

#include "KAI/Core/Type/Traits.h"

KAI_BEGIN

template <class T>
struct DerefType {
    using Value = T;
    using Tr = typename Type::Traits<T>;
    using Reference = typename Tr::Reference;
    using ConstReference = typename Tr::ConstReference;
};

template <class T> Storage<T>* Clone(StorageBase const& /*Q*/);

template <class T> DerefType<T>::ConstReference ConstDeref(StorageBase const& /*base*/);

::std::size_t GetHash(const ::kai::String &);

KAI_END
