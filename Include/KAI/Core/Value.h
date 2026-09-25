#pragma once

#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Object/GetStorageBase.h>
#include <KAI/Core/Type/Number.h>
#include <KAI/Core/Type/Traits.h>

KAI_BEGIN

// A ConstValue<> has direct access to the storage of an object.
// This means that accessing the value is faster as there is no
// lookup required. However, it is unsafe to use a Value over multiple
// frames, as the storage could be deleted by the garbage collector.
// So it is only really safe to use a ConstValue<> in local _scope,
// unless you can be sure that the corresponding Object will not
// be collected.
template <class T>
class ConstValue {
   protected:
       using Traits = Type::Traits<T>;
       using Store = typename Traits::Store;
       using Reference = typename Traits::Reference;
       using ConstReference = typename Traits::ConstReference;

       Storage<Store>* storage_;

   public:
       ConstValue() : storage_(nullptr) {}

       ConstValue(Object const& q) : storage_(nullptr)
       {
           AssignFrom(q);
       }

    operator Object() const {
        return *storage_;
    }

    ConstValue<T>& operator=(const ConstValue<T>& q)
    {
        AssignFrom(q.GetObject());
        return *this;
    }

    ConstValue<T>& operator=(Object const& q)
    {
        AssignFrom(q);
        return *this;
    }

    void AssignFrom(Object const& q)
    {
        storage_ = nullptr;

        // ULTIMATE defensive check - detect if source is not a valid address
        if (reinterpret_cast<uintptr_t>(&q) < 0x1000) {
            // This is an invalid pointer - it's pointing to a very low memory
            // address
            return;
        }

        try {
            // Check existence and validity with defensive error handling
            bool exists = false;
            bool valid = false;

            try {
                valid = q.Valid();
            } catch (...) {
                valid = false;
            }
            if (!valid) {
                return;
            }

            try {
                exists = q.Exists();
            } catch (...) {
                exists = false;
            }
            if (!exists) {
                return;
            }

            // Get type information with defensive error handling
            Type::Number type = Type::Number::None;
            try {
                type = q.GetTypeNumber();
            } catch (...) {
                return;
            }

            if (type == Type::Number::None) {
                return;
            }

            // Check type compatibility
            if (type != Type::Traits<T>::Number) {
                return; // Silently fail instead of throwing
            }

            // Final assignment with defensive error handling
            try {
                storage_ = &GetStorage<T>(q);
            } catch (...) {
                storage_ = nullptr;
            }
        } catch (...) {
            // Ultimate fallback - ensure storage is null
            storage_ = nullptr;
        }
    }

    ConstReference operator*() const {
        if (storage_ == 0) {
            KAI_THROW_0(NullObject);
        }
        return storage_->GetConstReference();
    }

    const T *operator->() const {
        if (storage_ == 0) {
            KAI_THROW_0(NullObject);
        }
        return &storage_->GetConstReference();
    }

    [[nodiscard]] Handle GetHandle() const
    {
        return Exists() ? GetObject().GetHandle() : Handle();
    }

    [[nodiscard]] Registry* GetRegistry() const
    {
        return Exists() ? GetObject().GetRegistry() : 0;
    }

    [[nodiscard]] Type::Number GetTypeNumber() const
    {
        return Exists() ? GetObject().GetTypeNumber() : Type::Number::None;
    }

    void SetSwitch(int s, bool m)
    {
        if (storage_) {
            storage_->SetSwitch(s, m);
        }
    }
    void SetMarked(bool m)
    {
        storage_->SetMarked(m);
    }
    [[nodiscard]] bool Valid() const
    {
        return Exists() ? GetObject().Valid() : false;
    }

    [[nodiscard]] bool Exists() const
    {
        if (storage_ == nullptr) {
            return false;
        }
        try {
            return GetObject().Exists();
        } catch (...) {
            return false;
        }
    }

    [[nodiscard]] bool IsManaged() const
    {
        return Exists() && storage_->IsManaged();
    }

    [[nodiscard]] bool IsConst() const
    {
        return Exists() && GetObject().IsConst();
    }

    [[nodiscard]] bool IsMutable() const
    {
        return Exists() && GetObject().IsMutable();
    }

    [[nodiscard]] Object& GetObject() const
    {
        if (storage_ == nullptr) {
            KAI_THROW_0(NullObject);
        }
        return *storage_;
    }

    [[nodiscard]] const Object& GetConstObject() const
    {
        if (storage_ == nullptr) {
            KAI_THROW_0(NullObject);
        }
        return *storage_;
    }
};

// A Value<> has direct access to the storage of an object.
// This means that accessing the value is faster as there is no
// lookup required. However, it is unsafe to use a Value over multiple
// frames, as the storage could be deleted by the garbage collector.
// So it is only really safe to use a Value<> in local _scope,
// unless you can be sure that the corresponding Object will not
// be collected.
template <class T>
struct Value : ConstValue<T> {
    using typename ConstValue<T>::Store;
    using typename ConstValue<T>::Reference;
    using typename ConstValue<T>::ConstReference;

    using ConstValue<T>::storage_;
    using ConstValue<T>::AssignFrom;

    Value() = default;

    Value(Object const& q)
    {
        AssignFrom(q);
    }

    Value<T>& operator=(Value<T> const& q)
    {
        ConstValue<T>::operator=(q);
        return *this;
    }

    Value<T>& operator=(Object const& q)
    {
        ConstValue<T>::operator=(q);
        return *this;
    }

    Reference operator*() {
        if (storage_ == 0) {
            KAI_THROW_0(NullObject);
        }
        return storage_->GetReference();
    }

    T *operator->() {
        if (storage_ == nullptr) {
            KAI_THROW_0(NullObject);
        }
        return &storage_->GetReference();
    }

    ConstReference operator*() const { return ConstValue<T>::operator*(); }

    const T *operator->() const { return ConstValue<T>::operator->(); }
};

template <class T>
struct Value<const T> : ConstValue<T> {
    using typename ConstValue<T>::Store;
    using typename ConstValue<T>::Reference;
    using typename ConstValue<T>::ConstReference;

    Value() = default;

    Value(Object const& q) : ConstValue<T>(q) {}

    Value<T>& operator=(Object const& q)
    {
        ConstValue<T>::operator=(q);
        return *this;
    }

    ConstReference operator*() const { return ConstValue<T>::operator*(); }

    const T *operator->() const { return ConstValue<T>::operator->(); }
};

KAI_END
