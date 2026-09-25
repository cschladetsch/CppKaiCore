#pragma once

#include <KAI/Core/Registry.h>
#include <KAI/Core/Type/Deref.h>
#include <KAI/Core/Type/Traits.h>

#include "KAI/Core/Object/ClassBase.h"
#include "KAI/Core/Object/GetStorageBase.h"
#include "KAI/Core/Object/Label.h"
#include "KAI/Core/Object/Object.h"
#include "KAI/Core/Object/ObjectConstructParams.h"
#include "KAI/Core/TriColor.h"
#include "KAI/Core/Value.h"

#undef RegisterClass

KAI_BEGIN

#pragma warning(push)

// warning C4702: unreachable code
#pragma warning(disable : 4702)

void MarkGrey(Object const &);

template <class T>
class Class : public ClassBase {
   public:
       using Traits = typename Type::Traits<T>;
       enum { Props = Traits::Props };

       Class(Label const& name) : ClassBase(name, Type::Traits<T>::Number) {}

       [[nodiscard]] int GetTraitsProperties() const override
       {
           return Props;
       }

       [[nodiscard]] bool HasTraitsProperty(int n) const override
       {
           return (Props & n) != 0;
       }

    // Lifetime management
       StorageBase* NewStorage(Registry* registry, Handle handle) const override
       {
           auto result =
               registry->GetMemorySystem().Allocate<Storage<T>>(ObjectConstructParams(registry, this, handle));

           if (!result.has_value()) {
               return nullptr;
           }

           Storage<T>* born = result.value();
           born->SetClean();
           return born;
       }

       void Create(StorageBase& storage) const override
       {
           CreateProperties(storage);
           Traits::LifetimeManager::Create(TypedStorage(storage));
       }

       bool Destroy(StorageBase& storage) const override
       {
           return Traits::LifetimeManager::Destroy(TypedStorage(storage));
       }

       void Delete(StorageBase& storage) const override
       {
           if (!properties_.empty()) {
               for (auto x : properties_) {
                   PropertyBase const& prop = *x.second;
                   if (!prop.IsSystemType()) {
                       continue;
                   }

                   Object k = prop.GetObject(storage);
                   if (k.Exists()) {
                       k.RemovedFromContainer(storage);
                   }
               }
           }
           Traits::LifetimeManager::Delete(TypedStorage(storage));
       }

    Storage<T> *TypedStorage(StorageBase &storage) const {
        return reinterpret_cast<Storage<T> *>(&storage);
    }

    void CreateProperties(StorageBase& object) const override
    {
        for (auto property : properties_) {
            PropertyBase const &prop = *property.second;
            if (!prop.IsSystemType()) {
                continue;
            }

            if (!prop.CreateDefaultValue()) {
                continue;
            }

            Object value = object.GetRegistry()->NewFromTypeNumber(
                prop.GetFieldTypeNumber());
            prop.SetObject(object, value);
        }
    }

    void Assign(StorageBase& a, StorageBase const& b) const override
    {
        Traits::Assign::Perform(Deref<T>(a), ConstDeref<T>(b));
    }

    [[nodiscard]] Object Duplicate(StorageBase const& parent) const override
    {
        Storage<T> *result = parent.GetRegistry()->NewStorage<T>();
        Traits::Assign::Perform(result->GetReference(), ConstDeref<T>(parent));
        // foreach (Properties::value_type const &property, properties_)
        for (auto property : properties_) {
            PropertyBase const &prop = *property.second;
            // if it is a system-type property, clone it, else just store the
            // value
            if (prop.IsSystemType()) {
                auto ch = prop.GetObject(parent);
                if (ch.Exists()) {
                    prop.SetObject(*result, ch.Clone());
                } else {
                    prop.SetObject(*result, Object());
                }
            } else {
                {
                    prop.SetValue(*result, prop.GetValue(parent));
                }
            }
        }
        return *result;
    }

    void GetContainedObjects(StorageBase& object, ObjectList& contained) const override
    {
        Traits::ContainerOps::ForEachContained(
            CleanDeref<T>(object), AddContainedFun<T, ObjectList>(contained));
    }

    void SetReferencedObjectsColor(StorageBase& q, ObjectColor::Color c, HandleSet& h) const override
    {
        ClassBase::SetReferencedObjectsColor(q, c, h);
        Traits::ContainerOps::ForEachContained(CleanDeref<T>(q), SetObjectColorRecursive<T>(c, h));
    }

    void SetSwitch(StorageBase& q, int s, bool m) const override
    {
        Traits::ContainerOps::SetSwitch(CleanDeref<T>(q), s, m);
    }

    void DetachFromContainer(StorageBase& q, Object const& k) const override
    {
        Traits::ContainerOps::Erase(Deref<T>(q), k);
    }

    void SetMarked2(StorageBase& q, bool m) const override
    {
        Traits::ContainerOps::SetMarked(CleanDeref<T>(q), m);
    }
    void MakeReachableGrey(StorageBase& base) const override
    {
        ClassBase::MakeReachableGrey(base);
        Traits::ContainerOps::ForEachContained(CleanDeref<T>(base),
                                               MakeReachableGreyFun<T>());
    }

    Object UpCast(StorageBase& q) const override
    {
        // This is almost always a bad idea. I see no reason to allow it in KAI
        // as well.
        KAI_NOT_IMPLEMENTED();
    }

    Object CrossCast(StorageBase& /*unused*/, Type::Number /*unused*/) const override
    {
        KAI_NOT_IMPLEMENTED();
    }

    Object DownCast(StorageBase& /*unused*/, Type::Number /*unused*/) const override
    {
        KAI_NOT_IMPLEMENTED();
    }

    [[nodiscard]] HashValue GetHashValue(const StorageBase& q) const override
    {
        return Traits::HashFunction::Calc(ConstDeref<T>(q));
    }

    [[nodiscard]] Object Absolute(const StorageBase& object) const override
    {
        Object result = object.Clone();
        Traits::Absolute::Perform(Deref<T>(object));
        return result;
    }

    [[nodiscard]] bool Boolean(const StorageBase& a) const override
    {
        return Traits::Boolean::Perform(ConstDeref<T>(a));
    }

    [[nodiscard]] bool Less(const StorageBase& a, const StorageBase& b) const override
    {
        return Traits::Less::Perform(ConstDeref<T>(a), ConstDeref<T>(b));
    }

    [[nodiscard]] bool Equiv(const StorageBase& a, const StorageBase& b) const override
    {
        if (!HasTraitsProperty(Type::Properties::Equiv) && HasTraitsProperty(Type::Properties::Less)) {
            return !Less(a, b) && !Less(b, a);
        }
        return Traits::Equiv::Perform(ConstDeref<T>(a), ConstDeref<T>(b));
    }

    [[nodiscard]] bool Greater(const StorageBase& a, const StorageBase& b) const override
    {
        return Traits::Greater::Perform(ConstDeref<T>(a), ConstDeref<T>(b));
    }

    [[nodiscard]] StorageBase* Plus(StorageBase const& a, StorageBase const& b) const override
    {
        Storage<T>* r = a.GetRegistry()->NewStorage<T>();
        Traits::Assign::Perform(r->GetReference(), Traits::Plus::Perform(ConstDeref<T>(a), ConstDeref<T>(b)));
        return r;
    }
    [[nodiscard]] StorageBase* Minus(StorageBase const& a, StorageBase const& b) const override
    {
        Storage<T>* r = a.GetRegistry()->NewStorage<T>();
        Traits::Assign::Perform(r->GetReference(), Traits::Minus::Perform(ConstDeref<T>(a), ConstDeref<T>(b)));
        return r;
    }

    [[nodiscard]] StorageBase* Multiply(StorageBase const& a, StorageBase const& b) const override
    {
        Storage<T>* r = a.GetRegistry()->NewStorage<T>();
        Traits::Assign::Perform(r->GetReference(), Traits::Multiply::Perform(ConstDeref<T>(a), ConstDeref<T>(b)));
        return r;
    }

    [[nodiscard]] StorageBase* Divide(StorageBase const& a, StorageBase const& b) const override
    {
        Storage<T>* r = a.GetRegistry()->NewStorage<T>();
        Traits::Assign::Perform(r->GetReference(), Traits::Divide::Perform(ConstDeref<T>(a), ConstDeref<T>(b)));
        return r;
    }

    void Insert(StringStream& s, const StorageBase& q) const override
    {
        Traits::StringStreamInsert::Insert(s, ConstDeref<T>(q));
    }

    StorageBase* Extract(Registry& r, StringStream& s) const override
    {
        Storage<T>* q = r.NewStorage<T>();
        Traits::StringStreamExtract::Extract(s, q->GetReference());
        return q;
    }

    void ExtractValue(Object& object, StringStream& strstream) const override
    {
        Traits::StringStreamExtract::Extract(strstream, Deref<T>(object));
    }

    void Insert(BinaryStream& s, const StorageBase& q) const override
    {
        Traits::BinaryStreamInsert::Insert(s, ConstDeref<T>(q));
    }

    StorageBase* Extract(Registry& r, BinaryStream& s) const override
    {
        Value<T> q = r.New<T>();
        Traits::BinaryPacketExtract::Extract(s, *q);
        return &q.GetObject().GetStorageBase();
    }
};

template <class T> Pointer<ClassBase const*> NewClass(Registry& r, const Label& name)
{
    auto klass = new Class<T>(name);
    return r.AddClass(Type::Traits<T>::Number, klass);
}

template <class T> Storage<T>* Clone(StorageBase const& q)
{
    auto dup = q.GetRegistry()->NewStorage<T>();
    dup->GetClass()->Clone(*dup, q);
    return dup;
}

#pragma warning(pop)

KAI_END
