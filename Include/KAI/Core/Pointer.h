#pragma once

#include <KAI/Core/Object/ClassBase.h>
#include <KAI/Core/Object/GetStorageBase.h>
#include <KAI/Core/Type/Traits.h>

KAI_BEGIN

/// Common base for ConstPointer<T> and Pointer<T>
/// To dereference a Pointer<T> requires a hash-table lookup.
template <class T>
class PointerBase {
   public:
       using Traits = Type::Traits<T>;
       using Reference = typename Traits::Reference;
       using ConstReference = typename Traits::ConstReference;
       using PointerType = typename Traits::Pointer;
       using ConstPointerType = typename Traits::ConstPointer;

   protected:
       bool CanAssign(const Object& q) const
       {
           if (!q.Exists()) {
               return true;
           }
           return CanAssign(q.GetStorageBase());
       }

       bool CanAssign(const StorageBase& p) const
       {
           Type::Number type = p.GetTypeNumber();
           if (type == Type::Number::None) {
               return true;
           }
           if (type == Type::Traits<Object>::Number) {
               return true;
           }
           if (type != Traits::Number) {
               KAI_THROW_2(TypeMismatch, Traits::Number, type.ToInt());
           }
           return true;
       }

#ifdef KAI_POINTER_HAS_STORAGEBASE
       mutable ConstPointerType pointer_;
#endif
};

template <>
struct PointerBase<Object> {
    using ConstPointerType = Type::Traits<Object>::ConstPointer;

    static bool CanAssign(const StorageBase& /*unused*/)
    {
        return true;
    }

#ifdef KAI_POINTER_HAS_STORAGEBASE
    mutable ConstPointerType pointer;
#endif
};

/// Provides const-only access to the underlying StorageBase
template <class T>
struct ConstPointer : PointerBase<T>, Object {
    using ConstReference = typename PointerBase<T>::ConstReference;
    using ConstPointerType = typename PointerBase<T>::ConstPointerType;
    using PointerBase<T>::CanAssign;
#ifdef KAI_POINTER_HAS_STORAGEBASE
    using PointerBase<T>::pointer_;
#endif
    // typedef typename PointerBase<T>::PointerType;

   protected:
       ConstPointer() = default;

       ConstPointer(const Object& q)
       {
           if (PointerBase<T>::CanAssign(q)) {
               Object::operator=(q);
           }
       }

       ConstPointer(const StorageBase* q)
       {
           if (q == nullptr) {
               return;
           }
           if (PointerBase<T>::CanAssign(*q)) {
               Object::operator=(*q);
           }
       }

   public:
       ConstPointer<T>& operator=(const ConstPointer<T>& x)
       {
           Object::operator=(x);
           return *this;
       }

    ConstReference operator*() const {
        ConstReference ref = ConstDeref<T>(*this);
#ifdef KAI_POINTER_HAS_STORAGEBASE
        pointer_ = &ref;
#endif
        return ref;
    }
    ConstPointerType operator->() const { return &**this; }

    ConstReference GetConstReference() const { return **this; }
};

/// Provides mutable access to the underlying storage
template <class T>
struct Pointer : PointerBase<T>, Object {
#ifdef KAI_POINTER_HAS_STORAGEBASE
    using PointerBase<T>::pointer_;
#endif

    Pointer() {
#ifdef KAI_POINTER_HAS_STORAGEBASE
        pointer_ = nullptr;
#endif
    }
    Pointer(const Object& q)
    {
        Pointer<T>::Assign(q);
    }

    explicit Pointer(StorageBase* q)
    {
        if (q == nullptr) {
            return;
        }
        if (q->IsConst()) {
            KAI_THROW_0(ConstError);
        }
        if (PointerBase<T>::CanAssign(*q)) {
            Assign(*q);
        }
    }
    Pointer<T>& operator=(const Pointer<T>& p)
    {
        Object::operator=(p);
#ifdef KAI_POINTER_HAS_STORAGEBASE
        pointer_ = nullptr;
        if (Exists()) {
            GetConstReference();
        }
#endif
        return *this;
    }

    using Reference = typename PointerBase<T>::Reference;
    using ConstReference = typename PointerBase<T>::ConstReference;
    using PointerType = typename PointerBase<T>::PointerType;
    using ConstPointerType = typename PointerBase<T>::ConstPointerType;

    Reference operator*() { return GetReference(); }
    PointerType operator->() { return &**this; }

    ConstReference operator*() const { return GetConstReference(); }
    ConstPointerType operator->() const { return &**this; }

    ConstReference GetConstReference() const {
        ConstReference ref = ConstDeref<T>(*this);
#ifdef KAI_POINTER_HAS_STORAGEBASE
        pointer_ = &ref;
#endif
        return ref;
    }
    Reference GetReference() const {
        Reference ref = Deref<T>(*this);
#ifdef KAI_POINTER_HAS_STORAGEBASE
        pointer_ = &ref;
#endif
        return ref;
    }

   protected:
       void Assign(const Object& q)
       {
           if (!PointerBase<T>::CanAssign(q)) {
               return;
           }
           if (q.GetHandle() != Handle(0) && q.Exists() && q.IsConst()) {
               KAI_THROW_0(ConstError);
           }
           Object::operator=(q);
#ifdef KAI_POINTER_HAS_STORAGEBASE
           if (Exists()) {
               GetConstReference();
           }
#endif
       }

   private:
    // do *not* define these
    Pointer(const Pointer<const T> &);
};

/// Special case to allow const pointers to be defined by type
template <class T>
struct Pointer<const T> : ConstPointer<T> {
    Pointer() = default;
    Pointer(Object const& q) : ConstPointer<T>(q) {}
    Pointer(const StorageBase* q) : ConstPointer<T>(q) {}
};

/// Special case to avoid type recursion
template <>
struct Pointer<Object> : Object {
    Pointer() = default;
    Pointer(Object const& q) : Object(q) {}
    Pointer(const StorageBase* q)
    {
        if (q == nullptr) {
            return;
        }
        Object::operator=(*q);
    }
    Object &operator*() { return *this; }
    Object *operator->() { return this; }
};

template <class T>
struct Pointer<Pointer<T> > : Pointer<T> {
    Pointer() = default;
    Pointer(Object const& q) : Pointer<T>(q) {}
    Pointer(const StorageBase* q) : Pointer<T>(q) {}
};

template <class T>
struct PointerType {
    using Type = Pointer<T>;
};

template <class T>
struct PointerType<Pointer<T> > {
    using Type = Pointer<T>;
};

template <class T>
struct PointerType<T &> {
    using Type = Pointer<T>;
};

template <class T>
struct PointerType<const T &> {
    using Type = Pointer<const T>;
};

template <class T>
struct ArgType {
    static T& From(Object const& p)
    {
        return Deref<T>(p);
    }
};

template <class T>
struct ArgType<const T> {
    static T From(Object const& p)
    {
        return ConstDeref<T>(p);
    }
};

template <class T>
struct ArgType<Pointer<T> > {
    static Pointer<T> const& From(Pointer<T> const& p)
    {
        return p;
    }
};

template <class T>
struct DerefType<Pointer<T> > : DerefType<T> {};

template <class T> DerefType<T>::Reference CleanDeref(StorageBase& base);

namespace Type {
// treat any use of Type::Traits<Pointer<T> > like Type::Traits<Object>
template <class T>
struct Traits<Pointer<T> > : Traits<Object> {};
}  // namespace Type

KAI_END
