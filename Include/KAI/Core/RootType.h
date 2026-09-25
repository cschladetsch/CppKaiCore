#pragma once

/// Strip all qualifiers from a given type.

template <class T>
struct RootType {
    using Type = T;
};

template <class T>
struct RootType<T &> {
    using Type = T;
};

template <class T>
struct RootType<const T &> {
    using Type = T;
};

template <class T>
struct RootType<T &&> {
    using Type = T;
};
