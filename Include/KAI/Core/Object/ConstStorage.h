#pragma once

#include <KAI/Core/Type/Traits.h>

#include "KAI/Core/Object/ObjectConstructParams.h"
#include "KAI/Core/Object/StorageBase.h"

KAI_BEGIN

template <class T>
class ConstStorage : public StorageBase {
    using Tr = typename Type::Traits<T>;
    using Store = typename Tr::Store;

protected:
    Store stored_;

public:
    ConstStorage(const ObjectConstructParams& p) : StorageBase(p) {} //{ SetClean(); }

    [[nodiscard]] Tr::ConstReference GetConstReference() const
    {
        return stored_;
    }
    Tr::ConstReference operator*() const
    {
        return stored_;
    }
    Tr::ConstPointer operator->() const
    {
        return &stored_;
    }
};

KAI_END
