#pragma once

#include <KAI/Core/Base.h>

KAI_BEGIN

// Indicates a range of characters in a larger string.
// Another humble but extremely useful structure.
struct Slice {
    int Start, End;

    Slice() { Start = End = 0; }
    Slice(int start, int end) : Start(start), End(end) {}

    [[nodiscard]] int Length() const
    {
        return End - Start;
    }

    friend bool operator==(Slice const& a, Slice const& b)
    {
        return a.Start == b.Start && a.End == b.End;
    }

    friend bool operator!=(Slice const& a, Slice const& b)
    {
        return !(a == b);
    }
};

KAI_END
