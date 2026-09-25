#pragma once

#include <KAI/Core/BuiltinTypes/String.h>
#include <KAI/Core/Type/TraitMacros.h>

KAI_BEGIN

struct Label {
    using Value = String;

private:
    Value value_;
    bool quoted_ = false;

   public:
    Label() = default;
    explicit Label(const String::Char* s)
    {
        FromString(s);
    }
    explicit Label(const Value& s)
    {
        FromString(s);
    }

    [[nodiscard]] bool Empty() const
    {
        return value_.Empty();
    }
    [[nodiscard]] bool Quoted() const
    {
        return quoted_;
    }
    void SetQuoted(bool q) { quoted_ = q; }

    void FromString(const Value& s);
    [[nodiscard]] String ToString() const;

    void FromString2(Value s);
    [[nodiscard]] const Value& GetValue() const
    {
        return value_;
    }

    friend bool operator<(const Label& a, const Label& b)
    {
        return a.ToString() < b.ToString();
    }

    friend bool operator==(const Label& a, const Label& b)
    {
        return a.ToString() == b.ToString();
    }

    static void Register(Registry &);
};

StringStream& operator<<(StringStream& s, const Label& l);
StringStream& operator>>(StringStream& s, Label& l);
BinaryStream &operator<<(BinaryStream &, Label const &);
BinaryStream &operator>>(BinaryStream &, Label &);
std::ostream &operator<<(std::ostream &, const Label &);

KAI_TYPE_TRAITS(Label, Number::Label,
                Properties::Streaming | Properties::Relational);

KAI_END

// #include "KAI/Core/LabelHash.h"

namespace boost {
inline size_t HashValue(KAI_NAMESPACE(Label) const& label)
{
    return HashValue(label.GetValue());
}
}  // namespace boost
