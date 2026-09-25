#pragma once

#include <KAI/Core/BuiltinTypes/Signed32.h>
#include <KAI/Core/Exception.h>
#include <KAI/Core/Object/ClassBuilder.h>

#include "Container.h"

KAI_BEGIN

/// A Map defines an associative container. Because this maps Object to Object,
/// it is possible to store mappings of any type to any type.
/// This is the base class for all mappings.
template <class Map>
struct MapBase : Container<Map> {
    using const_iterator = typename Map::const_iterator;
    using iterator = typename Map::iterator;
    using This = MapBase<Map>;

private:
    Map map_;

public:
    iterator Begin()
    {
        return map_.begin();
    }
    iterator End()
    {
        return map_.end();
    }

    [[nodiscard]] const_iterator Begin() const
    {
        return map_.begin();
    }
    [[nodiscard]] const_iterator End() const
    {
        return map_.end();
    }

    [[nodiscard]] int Size() const
    {
        return (int) map_.size();
    }
    [[nodiscard]] bool Empty() const
    {
        return map_.empty();
    }
    void Clear() {
        map_.clear();
    }
    void Insert(Object const &key, Object const &value) {
        map_[key] = value;
    }
    [[nodiscard]] bool ContainsKey(Object const& k) const
    {
        return Find(k) != End();
    }
    [[nodiscard]] const_iterator Find(Object const& k) const
    {
        return map_.find(k);
    }

    void Erase(Object const &key) {
        iterator a = map_.find(key);
        if (a == map_.end()) {
            KAI_THROW_1(UnknownKey, key);
        }
        a->first.SetColor(ObjectColor::White);
        a->second.SetColor(ObjectColor::White);
        map_.erase(a);
    }

    [[nodiscard]] Object GetValue(Object const& key) const
    {
        const_iterator a = map_.find(key);
        if (a == map_.end()) {
            return {}; // KAI_THROW_1(UnknownKey, key);
        }
        return a->second;
    }

    void SetChildSwitch(int s, bool m)
    {
        for (auto const& x : map_) {
            const_cast<Object&>(x.first).SetSwitch(s, m);
            x.second.SetSwitch(s, m);
        }
    }

    // friend bool operator<(const This &A, const This &B) { return A.map <
    // B.map; }
    friend bool operator==(const This& a, const This& b)
    {
        return a.map_ == b.map_;
    }

    static void Register(Registry& r, const char* n)
    {
        // TODO: Fix const correctness issues with Map methods
        // ClassBuilder<This>(R, Label(N))
        //    .Methods
        //    ("Size", &This::Size)
        //    ("Empty", &This::Empty)
        //    ("Insert", &This::Insert)
        //    ("Erase", &This::Erase)
        //    ("GetValue", &This::GetValue)
        //    ("ContainsKey", &This::ContainsKey)
        //    ;
    }
};

template <class Map> inline typename MapBase<Map>::iterator begin(MapBase<Map> &m) { return m.Begin(); }
template <class Map> inline typename MapBase<Map>::iterator end(MapBase<Map> &m) { return m.End(); }
template <class Map> inline typename MapBase<Map>::const_iterator begin(MapBase<Map> const &m) { return m.Begin(); }
template <class Map> inline typename MapBase<Map>::const_iterator end(MapBase<Map> const &m) { return m.End(); }

template <class Map> StringStream& operator<<(StringStream& s, MapBase<Map> const& m)
{
    s << "{ ";
    const char *sep = "";
    for (auto const& a : m) {
        s << sep << "[" << a.first << ", " << a.second << "]";
        sep = ", ";
    }
    return s << " }";
}

template <class Map> StringStream& operator>>(StringStream& s, MapBase<Map> const& m)
{
    // S << "{ ";
    // const char *sep = "";
    // for (auto const &A : M)
    // {
    //     S << sep << "[" << A.first << ", " << A.second << "]";
    //     sep = ", ";
    // }

    // this will need a parser....
    KAI_NOT_IMPLEMENTED();
}

template <class Map> BinaryStream& operator<<(BinaryStream& s, MapBase<Map> const& m)
{
    s << m.Size();
    for (const auto& a : m) {
        s << a.first << a.second;
    }
    return s;
}

template <class Map> BinaryStream& operator>>(BinaryStream& s, MapBase<Map>& m)
{
    int length = 0;
    s >> length;
    for (int n = 0; n < length; ++n) {
        Object key;
        Object value;
        s >> key >> value;
        m.Insert(key, value);
    }
    return s;
}

KAI_END
