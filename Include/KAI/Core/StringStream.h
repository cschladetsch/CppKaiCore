#pragma once

#include <KAI/Core/BuiltinTypes/String.h>
#include <KAI/Core/Config/Base.h>

#include <compare>
#include <string_view>
#include <vector>

KAI_BEGIN

struct Trace;

// Seems like a boring and unnecessary replication of std::stringstream.
// Rather, it allows insertion and extration of arbitrary registered objects
// from a string. It's probably one of the more interesting classes_ in the
// entire KAI system - by the mere fact that it works.
//
// A StringStream is itself a registered object, so they can act recursively
// and be addressed over a Network::Domain.
class StringStream {
   public:
       using Char = String::Char;
       using Storage = std::vector<Char>;

   private:
       Storage stream_;
       int readOffset_;
       Registry* registry_;

   public:
       StringStream() : readOffset_(0), registry_(nullptr) {}
       explicit StringStream(String const& s) : readOffset_(0), registry_(nullptr)
       {
           Append(s);
       }

       [[nodiscard]] const Storage& GetStorage() const
       {
           return stream_;
       }
       [[nodiscard]] String ToString() const;
       [[nodiscard]] bool Empty() const
       {
           return stream_.empty();
       }
       [[nodiscard]] int Size() const
       {
           return static_cast<int>(stream_.size());
       }
    void Clear() {
        stream_.clear();
    }
    [[nodiscard]] bool CanRead(int = 1) const;

    void SetRegistry(Registry* r)
    {
        registry_ = r;
    }
    void Append(Char);
    void Append(std::string_view);
    void Append(std::string_view a, std::string_view b);
    void Append(const String &);

    bool Extract(int length, String &);
    bool Extract(Char &);
    [[nodiscard]] char Peek() const;

    static void Register(Registry &);

    friend auto operator<=>(StringStream const &,
                            StringStream const &) = default;
    friend bool operator==(StringStream const &,
                           StringStream const &) = default;
};

struct EndsArgument {};
void Ends(EndsArgument);

StringStream &operator<<(StringStream &, void (*)(EndsArgument));
StringStream &operator<<(StringStream &, const String::Char *);

inline StringStream& operator<<(StringStream& s, const String& t)
{
    return s << t.CStr();
}
std::ostream& operator<<(std::ostream& s, const StringStream& t);
std::istream& operator>>(std::istream& s, StringStream& t);

KAI_END
