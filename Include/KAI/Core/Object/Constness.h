#pragma once

#include <KAI/Core/Base.h>

KAI_BEGIN

/// Specify the constness of a thing
struct Constness {
    enum Type { None, Const, Mutable };
    Type value;
    Constness(Type t = None) : value(t) {}

    [[nodiscard]] const char* ToString() const;
    friend bool operator==(Constness a, Constness b)
    {
        return a.value == b.value;
    }
    friend bool operator<(Constness a, Constness b)
    {
        return a.value < b.value;
    }
};

KAI_END
