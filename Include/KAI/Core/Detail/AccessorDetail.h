#pragma once

#include <KAI/Core/Object.h>
#include <KAI/Core/Object/AccessorBase.h>
#include <KAI/Core/Registry.h>
#include <KAI/Core/TypeInfo.h>

KAI_BEGIN

namespace property_detail {

// common functionality for accessors and mutators of either system or
// non-system types a `non-system` type is anything that is not convertible
// to an Object a `system type` is an Object, a Pointer<T>, a Pointer<const
// T>, or a Value<T> or Value<const T>
template <class Base, class C, class T, class F>
struct CommonBase : Base {
    using Field = F;
    Field field;
    CommonBase(Field g, Label l, bool b, member_create_params::Enum createParams)
        : Base(l, Type::Traits<C>::Number, Type::Traits<T>::Number, b, createParams), field(g)
    {
    }
};

// common functionality for accessors and mutators of non-system types.
template <class Base, class C, class T, class F>
struct NonSystemProperty : CommonBase<Base, C, T, F> {
    using Parent = CommonBase<Base, C, T, F>;
    using typename Parent::Field;

    NonSystemProperty(Field g, Label const& l, member_create_params::Enum createParams)
        : CommonBase<Base, C, T, F>(g, l, false, createParams)
    {
    }

    // cannot get or set the property object for non-system types
    [[nodiscard]] Object GetObject(Object const& /*unused*/) const
    {
        KAI_THROW_0(NoOperation);
    }
    void SetObject(Object const& /*unused*/, Object const& /*unused*/) const
    {
        KAI_THROW_0(NoOperation);
    }

    // non-system types cannot be marked
    void SetMarked(Object& /*unused*/, bool /*unused*/) {}
};

// common functionality for accessors and mutators of properties_ which are
// system types.
template <class Base, class C, class T, class F>
struct SystemProperty : CommonBase<Base, C, T, F> {
    using Parent = CommonBase<Base, C, T, F>;
    using Parent::field;
    using typename Parent::Field;

    SystemProperty(Field field, Label const& l, member_create_params::Enum createParams)
        : CommonBase<Base, C, T, F>(field, l, true, createParams)
    {
    }

    [[nodiscard]] Object GetObject(Object const& q) const
    {
        return Deref<C>(q).*field;
    }

    void SetObject(Object const &owner, Object const &value) const {
        Object &object = Deref<C>(owner).*field;
        if (object.Exists()) {
            object.RemovedFromContainer(owner);
        }

        if (!value.Valid()) {
            object = Object();
        } else {
            (object = value).AddedToContainer(owner);
        }
    }

    void SetMarked(Object& q, bool b)
    {
        MarkObjectAndChildren(Deref<C>(q).*field, b);
    }
};

// accessor to non-system properties_
template <class Class, class C, bool, class T, class = T>
struct Accessor : NonSystemProperty<AccessorBase, C, T, T(Class::*)> {
    using Parent = NonSystemProperty<AccessorBase, C, T, T(Class::*)>;
    using Parent::field;
    using typename Parent::Field;

    Accessor(Field f, Label const& l, member_create_params::Enum createParams)
        : NonSystemProperty<AccessorBase, C, T, T(Class::*)>(f, l, createParams)
    {
    }

    [[nodiscard]] Object GetValue(Object const& object) const
    {
        return object.GetRegistry()->New(ConstDeref<C>(object).*field);
    }
};

// accessor to system properties_
template <class Class, class C, class T, class S>
struct Accessor<Class, C, true, T, S>
    : SystemProperty<AccessorBase, C, T, S(Class::*)> {
    using Parent = SystemProperty<AccessorBase, C, T, S(Class::*)>;
    using Parent::field;
    using typename Parent::Field;

    Accessor(Field f, Label const& l, member_create_params::Enum createParams)
        : SystemProperty<AccessorBase, C, T, S(Class::*)>(f, l, createParams)
    {
    }

    [[nodiscard]] Object GetValue(Object const& object) const
    {
        return ConstDeref<C>(object).*field;
    }
};

// mutator for non-system types
template <class K, class Class, bool, class T, class S>
struct Mutator : NonSystemProperty<MutatorBase, K, T, T(Class::*)> {
    using Parent = NonSystemProperty<MutatorBase, K, T, T(Class::*)>;
    using Parent::field;
    using typename Parent::Field;

    Mutator(Field f, Label const& l, member_create_params::Enum createParams)
        : NonSystemProperty<MutatorBase, K, T, T(Class::*)>(f, l, createParams)
    {
    }

    [[nodiscard]] Object GetValue(Object const& object) const
    {
        return object.GetRegistry()->New(ConstDeref<K>(object).*field);
    }

    void SetValue(Object const &object, Object const &value) const {
        Deref<K>(object).*field = ConstDeref<T>(value);
    }
};

// mutator for system types
template <class K, class Class, class T, class S>
struct Mutator<K, Class, true, T, S>
    : SystemProperty<MutatorBase, K, T, S(Class::*)> {
    using Parent = SystemProperty<MutatorBase, K, T, S(Class::*)>;
    using Field = typename Parent::Field;
    using Parent::field;

    Mutator(Field field, Label const& label, member_create_params::Enum createParams)
        : SystemProperty<MutatorBase, K, T, S(Class::*)>(field, label, createParams)
    {
    }

    [[nodiscard]] Object GetValue(Object const& q) const
    {
        return ConstDeref<K>(q).*field; // just return the object for system types
    }

    void SetValue(Object const& q, Object const& v) const
    {
        if (!v.Exists()) {
            // Property was unset on the original object -- leave it unset here too.
            return;
        }

        Object object = Deref<K>(q).*field; // get the containing object
        if (!object.Exists()) {
            Deref<K>(q).*field = q.GetRegistry()->New(ConstDeref<T>(v));
            return;
        }

        object.Assign(object.GetStorageBase(),
                      v.GetStorageBase()); // use the class system to perform
                                           // the assignment
    }
};

// make an accessor (read-only) property
// Class is the type of the containing structure
// Class is the type of the structure which defines the member. this may be the
// same as Class. T is the type of the property
template <class K, class C, class T>
struct MakeAccessor
    : Accessor<K, C, TypeInfo<T>::IsSytem, typename TypeInfo<T>::ValueType,
               typename TypeInfo<T>::StorageType> {
    using Parent =
        Accessor<K, C, TypeInfo<T>::IsSytem, typename TypeInfo<T>::ValueType, typename TypeInfo<T>::StorageType>;
    using Field = typename Parent::Field;

    MakeAccessor(Field f, Label const& l, member_create_params::Enum createParams)
        : Accessor<K, C, TypeInfo<T>::IsSytem, typename TypeInfo<T>::ValueType, typename TypeInfo<T>::StorageType>(
              f, l, createParams)
    {
    }
};

// make a mutator (read-write) property
template <class K, class C, class T>
struct MakeMutator
    : Mutator<K, C, TypeInfo<T>::IsSytem, typename TypeInfo<T>::ValueType,
              typename TypeInfo<T>::StorageType> {
    using Parent =
        Mutator<K, C, TypeInfo<T>::IsSytem, typename TypeInfo<T>::ValueType, typename TypeInfo<T>::StorageType>;
    using Field = typename Parent::Field;

    MakeMutator(Field f, Label const& l, member_create_params::Enum createParams)
        : Mutator<K, C, TypeInfo<T>::IsSytem, typename TypeInfo<T>::ValueType, typename TypeInfo<T>::StorageType>(
              f, l, createParams)
    {
    }
};
}  // namespace property_detail

KAI_END
