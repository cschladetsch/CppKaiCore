#pragma once

KAI_BEGIN

namespace meta {
/// Null is used to represent the equivalent to a 'null-pointer' in compile-time
/// space
struct Null {
    using Next = Null;
    using Prev = Null;
    using Value = Null;
    using Type = Null;
};

/// Compile-time conditional evaluation.
template <bool T, class A, class B>
struct If {
    using Type = A;
};

template <class A, class B>
struct If<false, A, B> {
    using Type = B;
};

template <class A, class B>
struct SameType {
    enum { Value = 0 };
};

template <class A>
struct SameType<A, A> {
    enum { Value = 1 };
};

template <class T>
struct IsNull {
    enum { Value = 0 };
};

template <>
struct IsNull<Null> {
    enum { Value = 1 };
};

/// Caclulate the number of none-Null types in a list
template <class T0 = Null, class T1 = Null, class T2 = Null, class T3 = Null>
struct Arity {
    enum { Value = 4 };
};

template <class T0, class T1, class T2>
struct Arity<T0, T1, T2, Null> {
    enum { Value = 3 };
};

template <class T0, class T1>
struct Arity<T0, T1, Null, Null> {
    enum { Value = 2 };
};

template <class T0>
struct Arity<T0, Null, Null, Null> {
    enum { Value = 1 };
};

template <>
struct Arity<Null, Null, Null, Null> {
    enum { Value = 0 };
};
}  // namespace meta

KAI_END
