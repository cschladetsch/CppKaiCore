#pragma once

#include <KAI/Core/BuiltinTypes/Dictionary.h>
#include <KAI/Core/Type.h>

#include <list>

#include "KAI/Core/Object/Constness.h"
#include "KAI/Core/Object/Handle.h"
#include "KAI/Core/Object/ObjectConstructParams.h"
#include "KAI/Core/ObjectColor.h"

KAI_BEGIN

class Object {
    const ClassBase* classBase_{nullptr};
    Registry* registry_{nullptr};
    Handle handle_;

#ifdef KAI_CACHE_OBJECT_LOOKUPS
    // these fields are used to cache results for speed
    [[maybe_unused]] int gcIndex_{0};
    [[maybe_unused]] bool valid_{false};
    [[maybe_unused]] void* value_{nullptr};
#endif

   public:
    enum Switch {
        Marked = 1,
        Managed = 2,
        Const = 4,
        Clean = 8,
        // if set, when marking, will not mark children
        NoRecurse = 16,
        DefaultSwitches = Managed
    };

    Object() = default;
    Object(Object const &);
    explicit Object(ObjectConstructParams const& p);
    Object &operator=(Object const &);

    template <class T> [[nodiscard]] [[nodiscard]] [[nodiscard]] bool IsType() const
    {
        return Exists() && GetTypeNumber() == Type::Traits<T>::Number;
    }

    [[nodiscard]] StorageBase& GetStorageBase() const;
    [[nodiscard]] int GetSwitches() const;
    [[nodiscard]] ObjectColor::Color GetColor() const;
    void SetColor(ObjectColor::Color c) const;
    void SetColorRecursive(ObjectColor::Color c) const;
    void SetColorRecursive(ObjectColor::Color c, HandleSet&) const;
    [[nodiscard]] bool IsWhite() const
    {
        return GetColor() == ObjectColor::White;
    }
    [[nodiscard]] bool IsGrey() const
    {
        return GetColor() == ObjectColor::Grey;
    }
    [[nodiscard]] bool IsBlack() const
    {
        return GetColor() == ObjectColor::Black;
    }
    void SetWhite() const { SetColor(ObjectColor::White); }
    void SetGrey() const { SetColor(ObjectColor::Grey); }
    void SetBlack() const { SetColor(ObjectColor::Black); }
    [[nodiscard]] Object GetPropertyValue(Label const& l) const;
    [[nodiscard]] Type::Number GetTypeNumber() const;
    [[nodiscard]] const ClassBase* GetClass() const
    {
        return classBase_;
    }
    [[nodiscard]] Registry* GetRegistry() const
    {
        return registry_;
    }
    [[nodiscard]] Object Duplicate() const;
    [[nodiscard]] Object Clone() const
    {
        return Duplicate();
    }
    [[nodiscard]] Handle GetParentHandle() const;
    void SetParentHandle(Handle);
    [[nodiscard]] Handle GetHandle() const
    {
        return handle_;
    }
    [[nodiscard]] Object GetParent() const;
    void Delete() const;
    [[nodiscard]] bool Valid() const;
    [[nodiscard]] bool Exists() const;
    [[nodiscard]] bool OnDeathRow() const;
    [[nodiscard]] bool IsConst() const;
    [[nodiscard]] bool IsManaged() const;
    [[nodiscard]] bool IsMarked() const;
    [[nodiscard]] bool IsClean() const;
    void SetSwitch(int, bool) const;
    void SetSwitches(int) const;
    void SetMarked(bool = true) const;
    void SetConst() const;
    void SetManaged(bool = true) const;
    void SetClean(bool = true) const;
    [[nodiscard]] bool IsMutable() const
    {
        return !IsConst();
    }
    [[nodiscard]] bool IsUnmanaged() const
    {
        return !IsManaged();
    }
    [[nodiscard]] bool IsUnmarked() const
    {
        return !IsMarked();
    }
    [[nodiscard]] bool IsDirty() const
    {
        return !IsClean();
    }
    void Set(const char* label, const Object& q) const
    {
        Set(Label(label), q);
    }
    Object Get(const char *label) const { return Get(Label(label)); }
    void Add(const Label &label, const Object &child) const {
        Set(label, child);
    }
    void Set(const Label &, const Object &) const;
    [[nodiscard]] Object Get(const Label&) const;
    [[nodiscard]] bool Has(const Label&) const;
    void Remove(const Label &) const;
    void Detach(const Label& l) const
    {
        Remove(l);
    }
    void Detach(const Object& q) const;
    [[nodiscard]] Dictionary const& GetDictionary() const;
    void SetChild(const Label& l, const Object& q) const
    {
        Set(l, q);
    }
    [[nodiscard]] Object GetChild(const Label& l) const
    {
        return Get(l);
    }
    void RemoveChild(const Label& l) const
    {
        Remove(l);
    }
    void DetachChild(const Label& l) const
    {
        Remove(l);
    }
    void DetachChild(const Object& q) const
    {
        Detach(q);
    }
    [[nodiscard]] bool HasChild(const Label& l) const
    {
        return Has(l);
    }
    [[nodiscard]] Label GetLabel() const;
    [[nodiscard]] String ToString() const;
    [[nodiscard]] String ToXmlString() const;
    [[nodiscard]] Object NewFromTypeNumber(Type::Number n) const;
    void Assign(StorageBase &, StorageBase const &);
    [[nodiscard]] StorageBase* GetStorageBase(Handle other) const;
    void SetPropertyValue(Label const &, Object const &) const;
    void SetPropertyObject(Label const &, Object const &) const;
    [[nodiscard]] Object GetPropertyObject(Label const&) const;
    [[nodiscard]] bool HasProperty(Label const& name) const;
    static void Register(Registry &);
    void RemovedFromContainer(Object container) const;
    void AddedToContainer(Object container) const;
    [[nodiscard]] StorageBase* GetBasePtr() const;
    [[nodiscard]] StorageBase* GetParentBasePtr() const;
    using ObjectList = std::list<Object>;
    void GetPropertyObjects(ObjectList &contained) const;
    void GetContainedObjects(ObjectList &contained) const;
    void GetChildObjects(ObjectList &contained) const;
    void GetAllReferencedObjects(ObjectList &contained) const;

    [[nodiscard]] bool IsTypeNumber(int typeNumber) const
    {
        if (!Exists()) {
            return typeNumber == Type::Number::None;
        }

        return GetTypeNumber() == typeNumber;
    }

    class ChildProxy {
        friend class Object;
        Registry* registry_;
        Handle handle_;
        Label label_;
        Constness konst_;
        ChildProxy(Object const& q, const char*);
        ChildProxy(Object const& q, Label const& l);
        [[nodiscard]] Object GetObject() const;

    public:
        template <class T>
        ChildProxy &operator=(T const &value) {
            // GetObject().Set(
            throw;
            // return *this;
        }

        template <class T>
        ChildProxy &operator=(Pointer<T> const &value) {
            GetObject().Set(label_, value);
            return *this;
        }
        ChildProxy &operator=(Object const &child) {
            GetObject().Set(label_, child);
            return *this;
        }
        operator Object() const {
            return GetObject().Get(label_);
        }
    };

    ChildProxy operator[](const char *label) const {
        return ChildProxy(*this, label);
    }

   protected:
    Dictionary &GetDictionaryRef();
};

StringStream& operator<<(StringStream& s, const Object& q);
StringStream& operator>>(StringStream& s, Object& q);
BinaryStream& operator<<(BinaryStream& s, const Object& q);
BinaryStream& operator>>(BinaryStream& stream, Object& q);

bool operator<(Object const& a, Object const& b);
bool operator==(Object const& a, Object const& b);
inline bool operator!=(Object const& a, Object const& b)
{
    return !(a == b);
}
bool operator>(const Object& a, Object const& b);

Object operator+(Object const& a, Object const& b);
// WTF Object operator-(Object const &Object Absolute(Object const &A);

KAI_TYPE_TRAITS(Object, Number::Object,
                Properties::StringStreamInsert |
                    Properties::BinaryStreamInsert |
                    Properties::BinaryStreamExtract);

HashValue GetHash(Object const &);

void MarkObject(Object const &, bool = true);
void MarkObjectAndChildren(Object const &, bool = true);
void MarkObject(StorageBase &, bool = true);
void MarkObjectAndChildren(StorageBase &, bool = true);
Object Duplicate(Object const &);

KAI_END

namespace boost {
inline size_t HashValue(KAI_NAMESPACE(Object) const& h)
{
    return h.GetHandle().GetValue();
}
}  // namespace boost
