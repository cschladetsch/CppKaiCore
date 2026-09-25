#pragma once

#include <KAI/Core/Config/Base.h>

KAI_BEGIN

template <class T>
struct Argument {
    using Type = T&;
};

template <class T>
struct Argument<const T> {
    using Type = T&;
};

template <class T>
struct Argument<T &> {
    using Type = T&;
};

template <class T>
struct Argument<const T &> {
    using Type = T&;
};

KAI_END
