#pragma once

#include <memory>

#include "KAI/Core/Detail/AccessorDetail.h"
#include "KAI/Core/Object/Label.h"
#include "KAI/Core/Object/PropertyBase.h"
#include "KAI/Core/Pointer.h"

KAI_BEGIN

/// make a property which can only access the given class member
template <class K, class C, class T>
std::unique_ptr<PropertyBase> MakeProperty(T const(C::* f), Label const& n,
                                           member_create_params::Enum createParams = member_create_params::Default)
{
    return std::make_unique<property_detail::MakeAccessor<K, C, T>>(f, n, createParams);
}

/// make a property which can only access the given class member
template <class K, class C, class T>
std::unique_ptr<PropertyBase> MakeProperty(Pointer<const T> const(C::* f), Label const& n,
                                           member_create_params::Enum createParams = member_create_params::Default)
{
    return std::make_unique<property_detail::MakeAccessor<K, C, T>>(f, n, createParams);
}

/// make a property which can access and change the given class member
template <class K, class C, class T>
std::unique_ptr<PropertyBase> MakeProperty(T(C::* f), Label const& n,
                                           member_create_params::Enum createParams = member_create_params::Default)
{
    return std::make_unique<property_detail::MakeMutator<K, C, T>>(f, n, createParams);
}

// Deprecated compatibility functions
template <class K, class C, class T>
[[deprecated("Use MakeProperty that returns std::unique_ptr instead")]]
PropertyBase* MakePropertyRaw(T const(C::* f), Label const& n,
                              member_create_params::Enum createParams = member_create_params::Default)
{
    return new property_detail::MakeAccessor<K, C, T>(f, n, createParams);
}

template <class K, class C, class T>
[[deprecated("Use MakeProperty that returns std::unique_ptr instead")]]
PropertyBase* MakePropertyRaw(Pointer<const T> const(C::* f), Label const& n,
                              member_create_params::Enum createParams = member_create_params::Default)
{
    return new property_detail::MakeAccessor<K, C, T>(f, n, createParams);
}

template <class K, class C, class T>
[[deprecated("Use MakeProperty that returns std::unique_ptr instead")]]
PropertyBase* MakePropertyRaw(T(C::* f), Label const& n,
                              member_create_params::Enum createParams = member_create_params::Default)
{
    return new property_detail::MakeMutator<K, C, T>(f, n, createParams);
}

KAI_END
