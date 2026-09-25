#pragma once

#include <KAI/Core/Type/Number.h>

#include <utility>

#include "KAI/Core/BasePointer.h"
#include "KAI/Core/Object/Label.h"
#include "KAI/Core/Object/MemberCreateParams.h"
#include "KAI/UnfuckWin32.h"

KAI_BEGIN

/// nominates a field in an instance of a class. commonality for all accessors
/// and mutators.
class PropertyBase {
    Type::Number classType_;
    Type::Number fieldType_;
    Label fieldName_;

    /// True if this field is an Object or a Pointer<T> or convertable to either
    bool isSystem_;

    int createParams_;

public:
    PropertyBase(Label f, Type::Number c, Type::Number n, bool b, member_create_params::Enum cp)
        : fieldName_(std::move(f)), classType_(c), fieldType_(n), isSystem_(b), createParams_(cp)
    {
    }

    virtual ~PropertyBase() = default;

    String description;

    /// true if field is an Object or Pointer<T>
    [[nodiscard]] bool IsSystemType() const
    {
        return isSystem_;
    }

    /// if true, this property should be created when its containing object is
    /// created
    [[nodiscard]] bool CreateDefaultValue() const
    {
        return (createParams_ & member_create_params::Create) == member_create_params::Create;
    }

    virtual void SetMarked(Object &, bool) = 0;

    [[nodiscard]] virtual Object GetValue(Object const&) const = 0;
    virtual void SetValue(Object const &, Object const &) const = 0;

    virtual void SetObject(Object const& q, Object const& v) const = 0;
    [[nodiscard]] virtual Object GetObject(Object const& q) const = 0;

    [[nodiscard]] Label const& GetFieldName() const
    {
        return fieldName_;
    }
    [[nodiscard]] Type::Number GetClassTypeNumber() const
    {
        return classType_;
    }
    [[nodiscard]] Type::Number GetFieldTypeNumber() const
    {
        return fieldType_;
    }
};

StringStream& operator<<(StringStream& s, BasePointer<PropertyBase> const&);

KAI_TYPE_TRAITS(BasePointer<PropertyBase>, Number::Property,
                Properties::StringStreamInsert);

// access the property object
void SetPropertyObject(Object const &owner, Label const &name, Object value);
Object GetPropertyObject(Object const &owner, Label const &name);

// access the property value
void SetPropertyValue(Object const &owner, Label const &name, Object value);
Object GetPropertyValue(Object const &owner, Label const &name);

KAI_END
