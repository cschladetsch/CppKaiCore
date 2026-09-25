#pragma once

#include <KAI/Core/FwdDeclarations.h>

#include <memory>
#include <string>

KAI_BEGIN

class String {
   public:
       using Char = char;
       using Storage = typename std::basic_string<Char>;
       // typedef std::string Storage;
       using const_iterator = typename Storage::const_iterator;
       using iterator = typename Storage::iterator;

   private:
    Storage string_;

   public:
       String() = default;
       template <class II> String(II a, II b)
       {
           string_.assign(a, b);
       }
       String(const Char* s)
       {
           if (s != nullptr) {
               string_ = s;
           }
       }
       String(const std::string& X) : string_(X) {}
       String(const String& X) : string_(X.string_) {}
       //    String(String &&X) { string_ = std::move(X.string_); }
       explicit String(int n, Char c) : string_(n, c) {}

       friend bool operator==(String const& a, String const& b)
       {
           return a.string_ == b.string_;
       }

       friend bool operator!=(String const& a, String const& b)
       {
           return a.string_ != b.string_;
       }

       friend bool operator<(String const& a, String const& b)
       {
           return a.string_ < b.string_;
       }

       friend bool operator<=(String const& a, String const& b)
       {
           return a.string_ <= b.string_;
       }

       friend String& operator+=(String& a, Char b)
       {
           a.string_ += b;
           return a;
       }

       friend String& operator+=(String& a, String const& b)
       {
           a.string_ += b.string_;
           return a;
       }

    // friend String operator+(String &A, Char B)
    // {
    //     A.string_ += B;
    //     return std::move(A);
    // }

       friend String operator+(String const& a, String const& b)
       {
           return String(a.string_ + b.string_);
       }

    // We need this for the Boolean property to work properly
    explicit operator bool() const {
        return !Empty();
    }

    [[nodiscard]] const_iterator Begin() const
    {
        return string_.begin();
    }
    [[nodiscard]] const_iterator End() const
    {
        return string_.end();
    }

    iterator Begin()
    {
        return string_.begin();
    }
    iterator End()
    {
        return string_.end();
    }

    iterator Erase(iterator a, iterator b)
    {
        return string_.erase(a, b);
    }

    template <class II0, class II1> void Insert(II0 where, II1 begin, II1 end)
    {
        string_.insert(where, begin, end);
    }

    [[nodiscard]] int Size() const
    {
        return static_cast<int>(string_.size());
    }
    [[nodiscard]] bool Empty() const
    {
        return string_.empty();
    }
    [[nodiscard]] std::string StdString() const
    {
        return string_;
    }
    [[nodiscard]] const Storage& GetStorage() const
    {
        return string_;
    }
    void Clear()
    {
        string_.clear();
    }

    [[nodiscard]] String Tolower() const
    {
        return LowerCase();
    }
    String Toupper() const
    {
        return UpperCase();
    }

    [[nodiscard]] String LowerCase() const;
    [[nodiscard]] String UpperCase() const;
    [[nodiscard]] String Capitalise() const;

    [[nodiscard]] bool Contains(String const&) const;
    [[nodiscard]] bool StartsWith(String const&) const;
    [[nodiscard]] bool EndsWith(String const&) const;

    Char& operator[](int n)
    {
        return string_.at(n);
    }
    const Char& operator[](int n) const
    {
        return string_.at(n);
    }
    [[nodiscard]] const Char* CStr() const
    {
        return string_.c_str();
    }

    void ReplaceFirst(String const &what, String const &with);
    void ReplaceLast(String const &what, String const &with);
    void RemoveAll(String const &what);

    void Insert(Char const *);

    static void Register(Registry &);

    friend bool operator<(const String& a, const String& b);
    friend bool operator==(const String& a, const String& b);
    friend bool operator>(const String& a, const String& b);
};

StringStream& operator<<(StringStream& /*S*/, const String& /*T*/);
StringStream &operator>>(StringStream &, String &);
BinaryStream &operator<<(BinaryStream &, const String &);
BinaryStream &operator>>(BinaryStream &, String &);
std::ostream &operator<<(std::ostream &, const String &);

inline String::const_iterator begin(String const &s) { return s.Begin(); }
inline String::const_iterator end(String const &s) { return s.End(); }

KAI_END

namespace boost {
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) || \
    defined(_MSC_VER) || defined(__BORLANDC__) || defined(__TURBOC__)
#define GET16BITS(d) (*((const unsigned short*) (d)))
#endif

#ifndef get16bits
#define get16bits(d)                                      \
    ((((size_t)(((const unsigned char *)(d))[1])) << 8) + \
     (size_t)(((const unsigned char *)(d))[0]))
#endif
inline size_t HashValue(KAI_NAMESPACE(String) const& string)
{
    size_t len = string.Size();
    size_t hash = len;
    size_t tmp;
    if (len <= 0) {
        return 0;
    }
    const char* data = reinterpret_cast<const char*>(&*string.Begin());
    size_t rem = len & 3;
    len >>= 2;
    // Main loop; 4 bytes each iteration
    for (; len > 0; len--) {
        hash += GET16BITS(data);
        tmp = (GET16BITS(data + 2) << 11) ^ hash;
        hash = (hash << 16) ^ tmp;
        data += 4;
        hash += hash >> 11;
    }
    // handle end cases
    switch (rem) {
        case 3:
            hash += GET16BITS(data);
            hash ^= hash << 16;
            hash ^= data[sizeof(unsigned short)] << 18;
            hash += hash >> 11;
            break;
        case 2:
            hash += GET16BITS(data);
            hash ^= hash << 11;
            hash += hash >> 17;
            break;
        case 1:
            hash += *data;
            hash ^= hash << 10;
            hash += hash >> 1;
    }

    // Force "avalanching" of final 127 bits
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;
    return hash;
}
#undef get16bits
}  // namespace boost
