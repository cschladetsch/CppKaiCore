#pragma once

#include <KAI/Core/BuiltinTypes/Stack.h>
#include <KAI/Core/Config/Base.h>

#include "KAI/Core/CallableBase.h.notused"
#include "KAI/Core/Object/Constness.h"

KAI_BEGIN

/// Common for all methods_ that return void or not, and are const or not
class MethodBase : public CallableBase<MethodBase> {
   public:
       Type::Number classType;
       Constness constness;
       MethodBase(Constness c, const Label& n) : constness(c), CallableBase<MethodBase>(n) {}

       ~MethodBase() override = default;

       [[nodiscard]] Type::Number GetClassType() const
       {
           return classType;
       }
       [[nodiscard]] Constness GetConstness() const
       {
           return constness;
       }

       void Invoke(Object const& q, Stack& stack)
       {
           if (q.IsConst()) {
               ConstInvoke(q, stack);
           } else {
               NonConstInvoke(q, stack);
           }
       }

    virtual void NonConstInvoke(const Object &servant, Stack &stack) = 0;
    virtual void ConstInvoke(const Object &servant, Stack &stack) = 0;

    [[nodiscard]] Object GetArgumentTypesArray() const;
    [[nodiscard]] String ToString() const;
    static void Register(Registry &);
};

template <class Method>
struct ConstMethodBase : MethodBase {
    using MethodType = Method;
    Method method;
    ConstMethodBase(Method m, const Label& n) : method(m), MethodBase(Constness::Const, n) {}

    void NonConstInvoke(const Object& q, Stack& s) override
    {
        ConstInvoke(q, s);
    }
    ConstMethodBase(Method m, const Label& n, Constness c) : method(m), MethodBase(c, n) {}
};

template <class Method>
struct MutatingMethodBase : ConstMethodBase<Method> {
    MutatingMethodBase(Method m, const Label& n) : ConstMethodBase<Method>(m, n, Constness::Mutable) {}

    void ConstInvoke(const Object& /*unused*/, Stack& /*unused*/)
    {
        KAI_THROW_1(ConstError, "Mutating method");
    }
};

StringStream &operator<<(StringStream &, const BasePointer<MethodBase> &);

KAI_TYPE_TRAITS(BasePointer<MethodBase>, Number::Method,
                Properties::StringStreamInsert | Properties::Reflected);

KAI_END
