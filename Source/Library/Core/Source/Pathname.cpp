#include <ctype.h>

#include "KAI/Core/BuiltinTypes.h"

// NOTE: This should all be moved to the Lexer/Parser logic, not
// logic inherent in the Pathname class itself.
//
// Rather, the Lexer/Parser should generate Pathnames.
//
// The main point of contention is the ability to make Pathnames (and
// for that matter, Labels), "On the fly" in code without a lexer or parser.
//
// Note that Pi and Rho share the exact same syntax and semantics for Pathnames
// and Labels.

KAI_BEGIN

const String::Char Pathname::Literals::kParent = '^';
const String::Char Pathname::Literals::kThis = '.';
const String::Char Pathname::Literals::kSeparator = '/';
const String::Char Pathname::Literals::kQuote = '\'';
const String::Char Pathname::Literals::kAll[] = {Pathname::Literals::kParent, Pathname::Literals::kThis, Pathname::Literals::kSeparator, Pathname::Literals::kQuote,
                                                0};
const String::Char Pathname::Literals::kAllButQuote[] = {Pathname::Literals::kParent, Pathname::Literals::kThis, Pathname::Literals::kSeparator,
                                                        0};

Pathname::Pathname(const Elements &e) : elements_(e) {}

Pathname::Pathname(const String &text) { FromString(text); }

bool Pathname::Quoted() const {
    return !elements_.empty() && elements_.front().type == Element::Quote;
}

bool Pathname::Absolute() const {
    if (elements_.empty()) return false;

    if (Quoted())
        return elements_.size() > 1 && elements_[1].type == Element::Separator;

    return elements_.front().type == Element::Separator;
}

void Pathname::FromString2(String text) { FromString(text); }

// TODO: Pathnames and id's have been giving me grief for years.
// Need to sort it out once and for all.
// Need to use a static PiParser method or something. Doing it badly
// in three different places and across 3 different languages is insane.
void Pathname::FromString(const String &text) {
    elements_.clear();
    if (text.Empty()) return;

    const String::Char *S = text.CStr();

    StringStream name;
    for (; *S; ++S) {
        switch (*S) {
            case Literals::kQuote:
                elements_.push_back(Element::Quote);
                break;

            case Literals::kParent:
                AddElement(name, Element::Parent);
                break;

            case Literals::kSeparator:
                if (S[1] != 0) AddElement(name, Element::Separator);
                break;

            case Literals::kThis:
                AddElement(name, Element::This);
                break;

            default:
                if (!isalnum(*S) && *S != '_') {
                    elements_.clear();
                    KAI_THROW_1(InvalidPathname, text);
                }
                name.Append(*S);
                break;
        }
    }

    name << Ends;
    String s = name.ToString();
    if (!s.Empty()) elements_.push_back(Element(Label(s)));

    if (elements_.empty()) return;

    if (elements_.back().type == Element::Separator) elements_.pop_back();

    if (!Validate()) {
        elements_.clear();
        Validate();
        KAI_THROW_1(InvalidPathname, text);
    }
}

void Pathname::AddElement(StringStream &name, Element::Type type) {
    if (!name.Empty()) {
        name << Ends;
        elements_.push_back(Element(Label(name.ToString())));
    }

    elements_.push_back(type);
    name.Clear();
}

String Pathname::ToString() const {
    StringStream str;
    bool addedRoot = false;
    if (Absolute()) {
        addedRoot = true;
        str.Append(Literals::kSeparator);
    }

    for (auto element : elements_) {
        switch (element.type) {
            case Element::Quote:
                str.Append(Literals::kQuote);
                break;

            case Element::Separator:
                if (!addedRoot) str.Append(Literals::kSeparator);
                addedRoot = false;
                break;

            case Element::Parent:
                str.Append(Literals::kParent);
                break;

            case Element::This:
                str.Append(Literals::kThis);
                break;

            case Element::Name:
                str << element.name.ToString();
                break;

            case Element::None:
                break;
        }
    }

    str << Ends;
    return str.ToString();
}

bool Pathname::Empty() const { return elements_.empty(); }

bool Pathname::Validate() const {
    if (elements_.empty()) return true;
    // TODO
    return true;
}

StringStream &operator<<(StringStream &S, Pathname const &P) {
    return S << P.ToString();
}

bool operator<(const Pathname &A, const Pathname &B) {
    return A.elements_ < B.elements_;
}

bool operator==(const Pathname &A, const Pathname &B) {
    return A.elements_ == B.elements_;
}

BinaryPacket &operator>>(BinaryPacket &s, Pathname &p) {
    int size = 0;
    if (!s.Read(size) || size < 0 || !s.CanRead(size)) KAI_THROW_0(PacketExtraction);
    std::string text(static_cast<std::size_t>(size), '\0');
    if (size > 0 && !s.Read(size, text.data())) KAI_THROW_0(PacketExtraction);
    p = Pathname(String(text));
    return s;
}

BinaryStream &operator<<(BinaryStream &s, const Pathname &p) {
    const std::string text = p.ToString().StdString();
    const int size = static_cast<int>(text.size());
    s.Write(size);
    if (size > 0) s.Write(size, text.data());
    return s;
}

StringStream &operator>>(StringStream &, Pathname &) { KAI_NOT_IMPLEMENTED(); }

// Plus operation for Pathname - creates a combined pathname
Pathname operator+(const Pathname &A, const Pathname &B) {
    // Only allow addition if BOTH pathnames are quoted
    // Otherwise it's a type error (unquoted pathnames should resolve to values
    // first)
    if (!A.Quoted() || !B.Quoted()) {
        KAI_THROW_1(Base, "Cannot add pathnames unless both are quoted");
    }

    // Create a quoted pathname combining both
    // Get the path elements_ without the quote
    Pathname::Elements elemsA = A.GetElements();
    Pathname::Elements elemsB = B.GetElements();

    // Build new elements_ starting with quote
    Pathname::Elements newElems;
    newElems.push_back(Pathname::Element(Pathname::Element::Quote));

    // Add elements_ from A (skip quote if present)
    for (auto it = elemsA.begin(); it != elemsA.end(); ++it) {
        if (it == elemsA.begin() && it->type == Pathname::Element::Quote) {
            continue;  // Skip the quote
        }
        newElems.push_back(*it);
    }

    // Add separator if needed
    if (!newElems.empty() && newElems.back().type == Pathname::Element::Name) {
        newElems.push_back(Pathname::Element(Pathname::Element::Separator));
    }

    // Add elements_ from B (skip quote if present)
    for (auto it = elemsB.begin(); it != elemsB.end(); ++it) {
        if (it == elemsB.begin() && it->type == Pathname::Element::Quote) {
            continue;  // Skip the quote
        }
        newElems.push_back(*it);
    }

    return Pathname(newElems);
}

void Pathname::Register(Registry &R) {
    ClassBuilder<Pathname>(R, Label("Pathname"))
        .methods("Empty", &Pathname::Empty)("ToString", &Pathname::ToString)(
            "FromString", &Pathname::FromString2)(
            "absolute", &Pathname::Absolute)("quoted", &Pathname::Quoted);
}

KAI_END

// EOF
