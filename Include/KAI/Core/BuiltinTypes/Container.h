#pragma once

#include <KAI/Core/Object/Object.h>
#include <KAI/Core/Object/ObjectConstructParams.h>
#include <KAI/Core/Object/Reflected.h>
#include <KAI/Core/Object/Storage.h>

KAI_BEGIN

template <class T> struct Container : Reflected {
protected:
    Container() = default;

public:
    bool Attach(Object const& q)
    {
        if (self == nullptr) {
            // it is wrong to try to attach to a null container
            KAI_THROW_0(NullObject);
        }
        if (!q.Exists()) {
            // it is wrong to try to attach a null object to a container
            KAI_THROW_0(NullObject);
        }

        if (q.IsMarked()) {
            return false;
        }

        q.AddedToContainer(*self);
        return true;
    }
    void Detach(Object const& q)
    {
        if ((self == nullptr) || !q.Exists()) {
            return;
        }

        q.RemovedFromContainer(*self);
    }
    friend T;
};

template <class T>
class ConstStorage<Container<T> > : public StorageBase  //, IConstStorage<T>
{
    using Tr = typename Type::Traits<T>;
    using Stored = typename Tr::Store;

protected:
    Stored stored_;

public:
    ConstStorage(const ObjectConstructParams& p) : StorageBase(p)
    {
        SetClean();
    }

    Tr::ConstReference GetConstReference() const
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
