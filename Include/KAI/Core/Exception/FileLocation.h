#pragma once

#include <KAI/Core/BuiltinTypes/String.h>

KAI_BEGIN

/// A location in a source file represented as the file name_ and a line number
struct FileLocation {
    String file;
    String function;
    int line;

    FileLocation() : line(0) {}
    FileLocation(const char* f, int l, const char* g = "") : file(f), line(l), function(g) {}
    FileLocation(const char* g) : function(g), line(0) {}

    [[nodiscard]] String ToString() const;
    void AddLocation(StringStream &) const;
    void AddFunction(StringStream &) const;
};

KAI_END
