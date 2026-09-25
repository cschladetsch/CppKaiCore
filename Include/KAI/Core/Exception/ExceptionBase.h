#pragma once

#include <KAI/Core/Exception/FileLocation.h>
#include <KAI/Core/Object/Handle.h>
#include <KAI/Core/StringStream.h>

#include <exception>
#include <stdexcept>
#include <string>
#include <utility>

KAI_BEGIN

namespace exception
{
struct Base : std::logic_error {
    FileLocation location;
    std::string text;

    explicit Base(const char *const msg) : std::logic_error(msg), text(msg) {}
    explicit Base(FileLocation l, const char* t = "Exception") : std::logic_error(t), location(std::move(l)), text(t) {}

    [[nodiscard]] std::string ToString() const;
    virtual void WriteExtendedInformation(StringStream& /*unused*/) const {}
};

struct LogicError : Base {
    LogicError(const FileLocation &loc, const char *text) : Base(loc, text) {}
};

struct TypeMismatch : Base {
    int first, second;
    explicit TypeMismatch(const FileLocation& l, int a = 0, int b = 0, const char* t = "Type Mismatch")
        : Base(l, t), first(a), second(b)
    {
    }
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct NullObject : Base {
    explicit NullObject(const FileLocation& l, const char* t = "Null Object") : Base(l, t) {}
};

struct EmptyStack : Base {
    explicit EmptyStack(const FileLocation& l, const char* t = "Empty Stack") : Base(l, t) {}
};

struct ConstError : Base {
    explicit ConstError(const FileLocation& l, const char* t = "Const Error") : Base(l, t) {}
};
struct UnknownTypeNumber : Base {
    int typeNumber;
    explicit UnknownTypeNumber(const FileLocation& l, int n = 0, const char* t = "Unknown Type Number")
        : Base(l, t), typeNumber(n)
    {
    }
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct UnknownObject : Base {
    Handle handle;
    UnknownObject(const FileLocation& l, Handle h) : Base(l, "Unknown Object"), handle(h) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct ObjectNotFound : Base {
    String label;
    explicit ObjectNotFound(const FileLocation& l, const String& a = "") : Base(l, "ObjectNotFound"), label(a) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct Assertion : Base {
    explicit Assertion(const FileLocation& l) : Base(l, "AssertionFailed") {}
};

struct NoOperation : Base {
    int typeProperty;
    int typeNumber;
    NoOperation(const FileLocation& l)
        : Base(l, "Class does not have required operation"), typeProperty(0), typeNumber(0)
    {
    }
    NoOperation(const FileLocation& l, int n, int p)
        : Base(l, "Class does not have required opeation"), typeNumber(n), typeProperty(p)
    {
    }
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct PacketExtraction : Base {
    int typeNumber;
    explicit PacketExtraction(const FileLocation& l, int t = 0) : Base(l, "Cannot extract"), typeNumber(t) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct PacketInsertion : Base {
    int typeNumber;
    explicit PacketInsertion(const FileLocation& l, int t = 0) : Base(l, "Cannot insert"), typeNumber(t) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};

struct OutOfBounds : Base {
    int typeNumber;
    int index;

    OutOfBounds(const FileLocation& l, int idx, int ty) : Base(l, "Out of Bounds"), typeNumber(ty), index(idx) {}
    void WriteExtendedInformation(StringStream& /*unused*/) const override;
};
} // namespace exception

StringStream& operator<<(StringStream&, exception::Base const&);

KAI_END
