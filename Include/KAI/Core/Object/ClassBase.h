#pragma once

#include <KAI/Core/Base.h>
#include <KAI/Core/Exception/Extended.h>
#include <KAI/Core/Type/Number.h>
#include <KAI/Core/Type/Properties.h>

#include <list>
#include <unordered_map>
#include <utility>

#include "KAI/Core/Exception/ExceptionMacros.h"
#include "KAI/Core/Object/Label.h"
#include "KAI/Core/Object/LabelHash.h"
#include "KAI/Core/Object/MethodBase.h"
#include "KAI/Core/Object/PropertyBase.h"
#include "KAI/Core/StringStream.h"

KAI_BEGIN

class MethodBase;
class PropertyBase;

/// Base for all Class<T> types. ClassBase defines the type-independent
/// interface that all Class<T>s must define
class ClassBase {
   public:
       using Methods = std::unordered_map<Label, MethodBase*, detail::LabelHash>;
       using Properties = std::unordered_map<Label, PropertyBase*, detail::LabelHash>;
       using ObjectList = std::list<Object>;

   protected:
    Label name_;
    Methods methods_;
    Properties properties_;
    Type::Number typeNumber_;

public:
    ClassBase(Label name, Type::Number t) : name_(std::move(name)), typeNumber_(t) {}
    virtual ~ClassBase();

    [[nodiscard]] const Label& GetName() const
    {
        return name_;
    }
    [[nodiscard]] const Label& GetLabel() const
    {
        return GetName();
    }
    [[nodiscard]] Type::Number GetTypeNumber() const
    {
        return typeNumber_;
    }

    virtual void SetReferencedObjectsColor(StorageBase &base,
                                           ObjectColor::Color color,
                                           HandleSet &handles) const;

    void GetPropertyObjects(StorageBase &object, ObjectList &contained) const;

    /// methods_
    void AddMethod(const Label& l, MethodBase* m)
    {
        methods_[l] = m;
    }
    [[nodiscard]] const Methods& GetMethods() const
    {
        return methods_;
    }
    [[nodiscard]] MethodBase* GetMethod(const Label& l) const
    {
        const auto found = methods_.find(l);
        return found == methods_.end() ? nullptr : found->second;
    }

    /// properties_
    void AddProperty(Label const &label, PropertyBase *property) {
        properties_[label] = property;
    }
    [[nodiscard]] bool HasProperty(Label const& label) const
    {
        return properties_.contains(label);
    }

    [[nodiscard]] Properties const& GetProperties() const
    {
        return properties_;
    }

    [[nodiscard]] PropertyBase const& GetProperty(Label const& l) const
    {
        auto found = properties_.find(l);
        if (found == properties_.end()) {
            KAI_THROW_2(UnknownProperty, GetName(), l);
        }
        return *found->second;
    }

    [[nodiscard]] bool HasOperation(int n) const
    {
        return HasTraitsProperty(n);
    }

    virtual void MakeReachableGrey(StorageBase &base) const = 0;
    virtual bool CanBlackenReferencedObjects(StorageBase &base) const {
        return true;
    }
    virtual void GetContainedObjects(StorageBase &object,
                                     ObjectList &contained) const = 0;
    virtual void CreateProperties(StorageBase &object) const = 0;
    [[nodiscard]] virtual Object Duplicate(StorageBase const&) const = 0;
    virtual void DetachFromContainer(StorageBase &, Object const &) const = 0;
    [[nodiscard]] virtual int GetTraitsProperties() const = 0;
    [[nodiscard]] virtual bool HasTraitsProperty(int n) const = 0;
    virtual StorageBase *NewStorage(Registry *, Handle) const = 0;
    virtual void Create(StorageBase &) const = 0;
    virtual bool Destroy(StorageBase &) const = 0;
    virtual void Delete(StorageBase &) const = 0;
    virtual void Assign(StorageBase &, StorageBase const &) const = 0;
    virtual void SetSwitch(StorageBase& q, int s, bool m) const = 0;
    void SetMarked(StorageBase& q, bool m) const;
    virtual void SetMarked2(StorageBase& q, bool m) const = 0;
    virtual Object UpCast(StorageBase &) const = 0;
    virtual Object CrossCast(StorageBase &, Type::Number) const = 0;
    virtual Object DownCast(StorageBase &, Type::Number) const = 0;
    [[nodiscard]] virtual HashValue GetHashValue(const StorageBase&) const = 0;
    [[nodiscard]] virtual Object Absolute(const StorageBase&) const = 0;
    [[nodiscard]] virtual bool Less(const StorageBase&, const StorageBase&) const = 0;
    [[nodiscard]] virtual bool Greater(const StorageBase&, const StorageBase&) const = 0;
    [[nodiscard]] virtual bool Equiv(const StorageBase&, const StorageBase&) const = 0;
    [[nodiscard]] virtual bool Boolean(const StorageBase&) const = 0;
    virtual void Insert(StringStream &, const StorageBase &) const = 0;
    virtual StorageBase *Extract(Registry &, StringStream &) const = 0;
    virtual void ExtractValue(Object &object,
                              StringStream &strstream) const = 0;
    virtual void Insert(BinaryStream &, const StorageBase &) const = 0;
    virtual StorageBase *Extract(Registry &, BinaryStream &) const = 0;
    [[nodiscard]] virtual StorageBase* Plus(StorageBase const&, StorageBase const&) const = 0;
    [[nodiscard]] virtual StorageBase* Minus(StorageBase const&, StorageBase const&) const = 0;
    [[nodiscard]] virtual StorageBase* Multiply(StorageBase const&, StorageBase const&) const = 0;
    [[nodiscard]] virtual StorageBase* Divide(StorageBase const&, StorageBase const&) const = 0;

    [[nodiscard]] Object Absolute(Object const& object) const
    {
        return Absolute(object.GetStorageBase());
    }

    [[nodiscard]] bool Less(Object const& lhs, Object const& rhs) const
    {
        return Less(lhs.GetStorageBase(), rhs.GetStorageBase());
    }

    [[nodiscard]] bool Equiv(Object const& lhs, Object const& rhs) const
    {
        return Equiv(lhs.GetStorageBase(), rhs.GetStorageBase());
    }

    [[nodiscard]] bool Greater(Object const& lhs, Object const& rhs) const
    {
        return Greater(lhs.GetStorageBase(), rhs.GetStorageBase());
    }

    [[nodiscard]] bool Boolean(Object const& q) const
    {
        return Boolean(q.GetStorageBase());
    }

    void Assign(const Object& a, const Object& b) const
    {
        Assign(a.GetStorageBase(), b.GetStorageBase());
    }
    [[nodiscard]] Object Plus(const Object& a, const Object& b) const
    {
        return *Plus(a.GetStorageBase(), b.GetStorageBase());
    }
    [[nodiscard]] Object Minus(const Object& a, const Object& b) const
    {
        return *Minus(a.GetStorageBase(), b.GetStorageBase());
    }
    [[nodiscard]] Object Multiply(const Object& a, const Object& b) const
    {
        return *Multiply(a.GetStorageBase(), b.GetStorageBase());
    }
    [[nodiscard]] Object Divide(const Object& a, const Object& b) const
    {
        return *Divide(a.GetStorageBase(), b.GetStorageBase());
    }

    [[nodiscard]] bool Equiv2(const Object& a, const Object& b) const
    {
        return Equiv(a.GetStorageBase(), b.GetStorageBase());
    }
    [[nodiscard]] bool Less2(const Object& a, const Object& b) const
    {
        return Less(a.GetStorageBase(), b.GetStorageBase());
    }
    [[nodiscard]] bool Greater2(const Object& a, const Object& b) const
    {
        return Greater(a.GetStorageBase(), b.GetStorageBase());
    }
};

StringStream &operator<<(StringStream &, const ClassBase *);

KAI_TYPE_TRAITS_NAMED(const ClassBase *, Number::Class, "Class",
                      Properties::StringStreamInsert)

KAI_END
