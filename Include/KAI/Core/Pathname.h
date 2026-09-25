#pragma once

#include <KAI/Core/BuiltinTypes/String.h>
#include <KAI/Core/Type/Traits.h>

#include <list>
#include <utility>

#include "KAI/Core/Object/Label.h"

KAI_BEGIN

/// A Pathname represents a qualified name_ for an Object
class Pathname {
   public:
    struct Literals {
        static const String::Char kParent;
        static const String::Char kThis;
        static const String::Char kSeparator;
        static const String::Char kQuote;
        static const String::Char kAll[];
        static const String::Char kAllButQuote[];
    };

    struct Element {
        enum Type { None, Quote, Separator, Parent, This, Name };
        Type type;
        Label name;
        Element(Type t = None) : type(t) {}
        Element(Label l) : type(Name), name(std::move(l)) {}
        friend bool operator<(const Element& a, const Element& b)
        {
            return a.type < b.type || (a.type == b.type && a.name < b.name);
        }
        friend bool operator==(const Element& a, const Element& b)
        {
            return a.type == b.type && a.name == b.name;
        }
    };
    using Elements = std::vector<Element>;

private:
    Elements elements_;

public:
    Pathname() = default;
    Pathname(const String &);
    Pathname(const Elements &);

    [[nodiscard]] bool Quoted() const;
    [[nodiscard]] bool Absolute() const;

    [[nodiscard]] Elements GetElements() const
    {
        return elements_;
    }

    void FromString(const String &);
    void FromString2(String);
    [[nodiscard]] String ToString() const;

    [[nodiscard]] bool Empty() const;

    [[nodiscard]] Elements::const_iterator Begin() const
    {
        return elements_.begin();
    }
    [[nodiscard]] Elements::const_iterator End() const
    {
        return elements_.end();
    }

    friend bool operator<(const Pathname& a, const Pathname& b);
    friend bool operator==(const Pathname& a, const Pathname& b);
    friend Pathname operator+(const Pathname& a, const Pathname& b);

    static void Register(Registry &);

    [[nodiscard]] bool Validate() const;
    void AddElement(StringStream &, Element::Type);
};

StringStream &operator<<(StringStream &, Pathname const &);
StringStream &operator>>(StringStream &, Pathname &);
BinaryStream &operator<<(BinaryStream &, Pathname const &);
BinaryPacket &operator>>(BinaryPacket &, Pathname &);

template <class T> bool operator>(T const& a, T const& b)
{
    return b < a;
}

KAI_TYPE_TRAITS(Pathname, Number::Pathname,
                Properties::Streaming | Properties::Relational |
                    Properties::Plus);

Pathname GetFullname(const StorageBase &);
Pathname GetFullname(const Object &);
Label GetName(const Object &);

void Set(Object scope, const Pathname &path, StorageBase *);
void Set(Object scope, const Pathname& path, Object const& q);
void Set(Object const& root, Object const& scope, const Pathname& path, Object const& q);
void Set(Object const& root, Object const& scope, Object const& ident, Object const& q);

Object Get(Object scope, const Pathname &path);
Object Get(Object const &root, Object const &scope, const Pathname &path);
Object Get(Object const &root, Object const &scope, Object const &path);

void Remove(Object scope, const Pathname &path);
void Remove(Object const &root, Object const &scope, const Pathname &path);
void Remove(Object const &root, Object const &scope, Object const &ident);

bool Exists(Object const &root, Object const &scope, const Pathname &path);
bool Exists(Object const &scope, const Pathname &path);

KAI_END
