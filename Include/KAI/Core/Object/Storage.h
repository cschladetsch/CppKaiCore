#pragma once

#include "KAI/Core/Object/ConstStorage.h"

KAI_BEGIN

template <class T>
class Storage : public ConstStorage<T> {
   public:
       Storage(const ObjectConstructParams& p) : ConstStorage<T>(p) {}

       using Tr = typename Type::Traits<T>;

       Tr::Reference GetReference()
       {
           StorageBase::SetDirty();
           return ConstStorage<T>::stored_;
       }

       Tr::Reference operator*() /*const*/
       {
           return GetReference();
       }
       Tr::Pointer operator->() /*const*/
       {
           return &GetReference();
       }

       Tr::Reference GetCleanReference()
       {
           return ConstStorage<T>::stored_;
       }
};

template <class T>
class Storage<const T> : ConstStorage<T> {
    Storage(const ObjectConstructParams& p) : ConstStorage<T>(p) {}
    using Tr = typename ConstStorage<T>::Traits;

    Tr::Reference GetReference()
    {
        KAI_THROW_0(ConstError);
    }
};

KAI_END
