#pragma once

#include <utility>

#include "KAI/Core/Object/Object.h"
#include "KAI/Core/Pathname.h"

KAI_BEGIN

namespace exception
{
struct CannotResolve : Base {
    Object object;
    Label label;
    Pathname path;
    CannotResolve(const FileLocation& l, Object const& q) : Base(l, "CannotResolve"), object(q) {}
    CannotResolve(const FileLocation& l, Label q) : Base(l, "CannotResolve"), label(std::move(q)) {}
    CannotResolve(const FileLocation& l, Pathname q) : Base(l, "CannotResolve"), path(std::move(q)) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct InvalidPathname : Base {
    String text;
    InvalidPathname(const FileLocation& l, const String& t) : Base(l, "InvalidPathname"), text(t) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct InvalidIdentifier : Base {
    Object what;
    InvalidIdentifier(const FileLocation& l, const Object& t) : Base(l, "InvalidIdentifier"), what(t) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct NotImplemented : Base {
    String text;
    NotImplemented(const FileLocation& l, const char* t = "<unnamed>") : Base(l, "Not Implemented"), text(t) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct ObjectNotInTree : Base {
    Object object;
    ObjectNotInTree(const FileLocation& l, Object const& q) : Base(l, "Object not in tree"), object(q) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct InternalError : Base {
    InternalError(const FileLocation& l, const char* t) : Base(l, t) {}
};

struct UnknownMethod : Base {
    String name;
    String className;
    UnknownMethod(const FileLocation& l, const String& n, const String& b)
        : Base(l, "Unknown Method"), name(n), className(b)
    {
    }
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct UnknownHandle : Base {
    Handle handle;
    UnknownHandle(const FileLocation& l, Handle n) : Base(l, "Unknown Handle"), handle(n) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct InvalidStringLiteral : Base {
    String text;
    InvalidStringLiteral(const FileLocation& l, const String& t) : Base(l, "Invalid string literal"), text(t) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

template <class T = meta::Null>
struct UnknownClass : Base {
    String name;
    Type::Number typeNumber;
    UnknownClass(const FileLocation& l, const String& n) : Base(l, "Unknown Class"), name(n) {}
    UnknownClass(const FileLocation& l, Type::Number n) : Base(l, "Unknown Class"), typeNumber(n) {}
    UnknownClass(const FileLocation& l)
        : Base(l, "Unknown Class"), typeNumber(Type::Traits<T>::Number), name("TODO boost::typeindex")
    {
    }
    void WriteExtendedInformation(StringStream& s) const override
    {
        s << "name_=" << name << ", type_number_=" << typeNumber.value;
    }
};

struct CannotNew : Base {
    Object arg;
    CannotNew(const FileLocation& l, const Object& q) : Base(l, "Cannot new"), arg(q) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct AssertionFailed : Base {
    AssertionFailed(const FileLocation& l) : Base(l, "AssertionFailed") {}
};

struct UnknownKey : Base {
    Object key;
    UnknownKey(const FileLocation& l, const Object& k) : Base(l, "Unknown key in Map"), key(k) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct BadIndex : Base {
    int index;
    BadIndex(const FileLocation& l, int n) : Base(l, "Bad Index"), index(n) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct BadUpCast : Base {
    BadUpCast(const FileLocation& l) : Base(l, "BadUpCast") {}
};

struct FileNotFound : Base {
    String filename;
    FileNotFound(const FileLocation& l, String const& f) : Base(l, "FileNotFound"), filename(f) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct UnknownProperty : Base {
    Label klass;
    Label prop;
    UnknownProperty(const FileLocation& l, Label k, Label p)
        : Base(l, "UnknownProperty"), klass(std::move(k)), prop(std::move(p))
    {
    }
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct NoInput : Base {
    NoInput(const FileLocation& l) : Base(l, "UnknownProperty") {}
};

struct DivideByZero : Base {
    DivideByZero(const FileLocation& l) : Base(l, "DivideByZero") {}
};

} // namespace exception

KAI_END
