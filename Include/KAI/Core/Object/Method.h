#pragma once

#include <KAI/Core/Detail/Arity.h>
#include <KAI/Core/Meta/Base.h>

#include <memory>

#include "KAI/Core/Detail/CallableBase.h"
#include "MethodBase.h"

KAI_BEGIN

namespace method_detail {
// using namespace std; // removed: pollutes kai namespace
using namespace meta;
using namespace detail;

// a method that returns something and is const
template <class T, class R, class... Args>
struct MethodConst : ConstMethodBase<R (T::*)(Args...) const> {
    using MethodType = R (T::*)(Args...) const;
    using Parent = ConstMethodBase<MethodType>;
    MethodType meth;
    std::tuple<std::decay_t<Args>...> args;
    static int constexpr kArity = (int) sizeof...(Args);

    MethodConst(MethodType m, const Label& n) : meth(m), Parent(m, n) {}

    void ConstInvoke(const Object &servant, Stack &stack) {
        if constexpr (kArity > 0) {
            detail::Add<kArity - 1>::Arg(stack, args);
        }
        stack.Push(servant.GetRegistry()->New(CallMethod(ConstDeref<T>(servant), meth, args)));
    }
};

// a method that returns void and is const
template <class T, class... Args>
struct VoidMethodConst : ConstMethodBase<void (T::*)(Args...) const> {
    using MethodType = void (T::*)(Args...) const;
    using Parent = ConstMethodBase<MethodType>;
    static std::size_t constexpr kArity = sizeof...(Args);
    MethodType meth;
    tuple<std::decay_t<Args>...> args;
    VoidMethodConst(MethodType mb, const Label& n) : meth(mb), Parent(mb, n) {}

    void ConstInvoke(const Object &servant, Stack &stack) {
        if constexpr (kArity > 0) {
            detail::Add<kArity - 1>::Arg(stack, args);
        }
        CallMethod(ConstDeref<T>(servant), meth, args);
    }
};

// a method that returns void  and is not const
template <class T, class... Args>
struct VoidMethod : MutatingMethodBase<void (T::*)(Args...)> {
    using MethodType = void (T::*)(Args...);
    using Parent = MutatingMethodBase<MethodType>;
    static std::size_t constexpr kArity = sizeof...(Args);
    MethodType meth;
    std::tuple<std::decay_t<Args>...> args;

    VoidMethod(MethodType m, const Label& n) : meth(m), Parent(m, n) {}

    void NonConstInvoke(const Object &servant, Stack &stack) override {
        if constexpr (kArity > 0) {
            detail::Add<kArity - 1>::Arg(stack, args);
        }
        CallMethod(Deref<T>(const_cast<Object&>(servant)), meth, args);
    }
};

// a method that returns something and is not const
template <class T, class R, class... Args>
struct Method : MutatingMethodBase<R (T::*)(Args...)> {
    using MethodType = R (T::*)(Args...);
    using Parent = MutatingMethodBase<MethodType>;
    static std::size_t constexpr kArity = sizeof...(Args);
    MethodType meth;
    std::tuple<std::decay_t<Args>...> args;

    Method(MethodType m, const Label& n) : meth(m), Parent(m, n) {}

    void NonConstInvoke(const Object &servant, Stack &stack) override {
        if constexpr (kArity > 0) {
            detail::Add<kArity - 1>::Arg(stack, args);
        }
        auto& nonConstServant = const_cast<Object&>(servant);
        auto result = CallMethod(Deref<T>(nonConstServant), meth, args);
        stack.Push(nonConstServant.GetRegistry()->New(result));
    }
};

template <bool V, bool C, class T00, class T01, class T10, class T11>
struct Select {
    using Type = typename If<V, typename If<C, T11, T10>::Type, typename If<C, T01, T00>::Type>::Type;
};

/// selects implementation based on whether the target method
/// returns void or not
template <class T, class R, bool C, class... Args>
struct Selector {
    enum { VoidRet = SameType<R, void>::Value };
    enum { Const = C };

    using Type = typename Select<VoidRet, Const, Method<T, R, Args...>, MethodConst<T, R, Args...>,
                                 VoidMethod<T, Args...>, VoidMethodConst<T, Args...>>::Type;
};

}  // namespace method_detail

template <class T, class R, bool C, class... Args>
struct Method : method_detail::Selector<T, R, C, Args...>::Type {
    using Parent = typename method_detail::Selector<T, R, C, Args...>::Type;
    Method(Parent::MethodType m, const Label& l) : Parent(m, l) {}
};

template <class T, class R, class... Args>
std::unique_ptr<MethodBase> MakeMethod(R (T::*method)(Args...), const Label& n)
{
    return std::make_unique<Method<T, R, false, Args...>>(method, n);
}

template <class T, class R, class... Args>
std::unique_ptr<MethodBase> MakeMethod(R (T::*method)(Args...) const, const Label& n)
{
    return std::make_unique<Method<T, R, true, Args...>>(method, n);
}

// Deprecated compatibility functions
template <class T, class R, class... Args>
[[deprecated("Use MakeMethod returning unique_ptr instead")]]
MethodBase* MakeMethodRaw(R (T::*method)(Args...), const Label& n)
{
    return new Method<T, R, false, Args...>(method, n);
}

template <class T, class R, class... Args>
[[deprecated("Use MakeMethod returning unique_ptr instead")]]
MethodBase* MakeMethodRaw(R (T::*method)(Args...) const, const Label& n)
{
    return new Method<T, R, true, Args...>(method, n);
}

KAI_END
