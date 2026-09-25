#pragma once

#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Object/Object.h>
#include <KAI/Core/Type/Number.h>

#include "KAI/Core/Object/Label.h"
#include "KAI/Core/Object/PropertyBase.h"

KAI_BEGIN

class AccessorBase : public PropertyBase {
   public:
       AccessorBase(Label const& f, Type::Number c, Type::Number n, bool isSystem, member_create_params::Enum cp)
           : PropertyBase(f, c, n, isSystem, cp)
       {
       }

       void SetValue(Object const& /*unused*/, Object const& /*unused*/) const override
       {
           KAI_THROW_0(ConstError);
       }
};

class MutatorBase : public AccessorBase {
   public:
       MutatorBase(Label const& f, Type::Number c, Type::Number n, bool isSystem, member_create_params::Enum cp)
           : AccessorBase(f, c, n, isSystem, cp)
       {
       }

       [[nodiscard]] Object GetValue(Object const& q) const override
       {
           if (!IsSystemType()) {
               KAI_THROW_0(ConstError);
           }

           return GetPropertyObject(q, GetFieldName());
       }
};

KAI_END
