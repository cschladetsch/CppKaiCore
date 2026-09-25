#pragma once

#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Type/TraitMacros.h>

KAI_BEGIN

struct Vector2 {
    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}
    float x, y;
};

struct Vector3 {
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    float x, y, z;
};

struct Vector4 {
    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    float x, y, z, w;
    Vector4 &operator*=(float a) {
        x *= a;
        y *= a;
        z *= a;
        w *= a;
        return *this;
    }
    Vector4 &operator/=(float a) {
        x /= a;
        y /= a;
        z /= a;
        w /= a;
        return *this;
    }
    Vector4& operator+=(Vector4 const& r)
    {
        x += r.x;
        y += r.y;
        z += r.z;
        w += r.z;
        return *this;
    }
    Vector4& operator-=(Vector4 const& r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;
        w -= r.w;
        return *this;
    }
    friend Vector4 operator*(Vector4 const& a, float b)
    {
        Vector4 r(a);
        r *= b;
        return r;
    }
    friend Vector4 operator/(Vector4 const& a, float b)
    {
        Vector4 r(a);
        r /= b;
        return r;
    }
    friend Vector4 operator+(Vector4 const& a, Vector4 const& b)
    {
        Vector4 r(a);
        r += b;
        return r;
    }
    friend Vector4 operator-(Vector4 const& a, Vector4 const& b)
    {
        Vector4 r(a);
        r -= b;
        return r;
    }
};

StringStream &operator<<(StringStream &, Vector2 const &);
StringStream &operator>>(StringStream &, Vector2 &);
BinaryStream &operator<<(BinaryStream &, Vector2 const &);
BinaryStream &operator>>(BinaryStream &, Vector2 &);

StringStream &operator<<(StringStream &, Vector3 const &);
StringStream &operator>>(StringStream &, Vector3 &);
BinaryStream &operator<<(BinaryStream &, Vector3 const &);
BinaryStream &operator>>(BinaryStream &, Vector3 &);

StringStream &operator<<(StringStream &, Vector4 const &);
StringStream &operator>>(StringStream &, Vector4 &);
BinaryStream &operator<<(BinaryStream &, Vector4 const &);
BinaryStream &operator>>(BinaryStream &, Vector4 &);

KAI_TYPE_TRAITS(Vector2, Number::Vector2,
                Properties::Streaming | Properties::Assign
                //| Properties::Plus
                //| Properties::Minus
                //| Properties::Absolute
);

KAI_TYPE_TRAITS(Vector3, Number::Vector3,
                Properties::Streaming | Properties::Assign
                //| Properties::Plus
                //| Properties::Minus
                //| Properties::Absolute
);

KAI_TYPE_TRAITS(Vector4, Number::Vector4,
                Properties::Streaming | Properties::Plus
                //| Properties::Minus
                //| Properties::Assign
                //| Properties::Absolute
);

KAI_END
