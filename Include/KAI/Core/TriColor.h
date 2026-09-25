#pragma once

#include <KAI/Core/BuiltinTypes/String.h>
#include <KAI/Core/Config/Base.h>

#include <utility>

#include "KAI/Core/Object/Handle.h"
#include "KAI/Core/Object/Object.h"
#include "KAI/Core/Object/StorageBase.h"
#include "KAI/Core/ObjectColor.h"

KAI_BEGIN

template <class Cont, class Fun>
Fun ForEach(Cont &container, Fun fun) {
    for (auto& a : container) {
        if (!fun(a)) {
            break;
        }
    }
    return fun;
}

template <class T>
struct Deleter  // : Function<void (*)(T *)
{
    bool operator()(T* p)
    {
        delete p;
        return true;
    }
    bool operator()(const T* p)
    {
        delete const_cast<T*>(p);
        return true;
    }
    template <class A, class B>
    bool operator()(const std::pair<A, B *> &pair) {
        delete pair.second;
        return true;
    }
};

template <class T>
struct IteratedFunctionBase {
    // return false to cease the iteration
    virtual bool Invoke(Object const& q) = 0;

    bool operator()(Object const& q)
    {
        if (!q.Exists()) {
            return true;
        }
        return Invoke(q);
    }
    bool operator()(Object& q)
    {
        if (!q.Exists()) {
            return true;
        }
        return Invoke(q);
    }
    bool operator()(std::pair<Object, Object>& m)
    {
        return Invoke(m.first) && Invoke(m.second);
    }
    bool operator()(std::pair<const Object, Object>& m)
    {
        return Invoke(const_cast<Object&>(m.first)) && Invoke(m.second);
    }
    bool operator()(std::pair<const String, Object>& m)
    {
        return Invoke(m.second);
    }
    template <class T2> bool operator()(std::pair<const String, Pointer<T2>>& m)
    {
        return Invoke(m.second);
    }
};

template <class T>
struct SetSwitch : IteratedFunctionBase<T> {
    int val;
    bool on;
    SetSwitch(int v, bool q) : val(v), on(q) {}
    bool Invoke(Object const& object) override
    {
        object.SetSwitch(val, on);
        return true;
    }
};

template <class T>
struct SetMarked : IteratedFunctionBase<T> {
    bool mark;
    SetMarked(bool q) : mark(q) {}
    bool Invoke(Object const& m)
    {
        MarkObjectAndChildren(m, mark);
        return true;
    }
};

template <class T>
struct CanBlackenFun : IteratedFunctionBase<T> {
    bool can_make_black{true};
    CanBlackenFun() {}
    bool Invoke(Object const& q)
    {
        StorageBase* base = q.GetStorageBase(q.GetHandle());
        return can_make_black = can_make_black && (base != nullptr) && !base->IsWhite();
    }
};

template <class T>
struct SetObjectColor : IteratedFunctionBase<T> {
    ObjectColor::Color color;
    SetObjectColor(ObjectColor::Color c) : color(c) {}
    bool Invoke(Object const& q)
    {
        Object(q).SetColor(color);
        return true;
    }
};

template <class T>
struct SetObjectColorRecursive : IteratedFunctionBase<T> {
    ObjectColor::Color color;
    HandleSet *handles;
    SetObjectColorRecursive(ObjectColor::Color c, HandleSet& h) : color(c), handles(&h) {}

    bool Invoke(Object const& q)
    {
        Object(q).SetColorRecursive(color, *handles);
        return true;
    }
};

template <class T>
struct MakeReachableGreyFun : IteratedFunctionBase<T> {
    bool Invoke(Object const& q)
    {
        StorageBase* base = q.GetStorageBase(q.GetHandle());
        if (base != nullptr && base->IsWhite()) {
            base->SetColor(ObjectColor::Grey);
        }
        return true;
    }
};

template <class T, class C>
struct AddContainedFun : IteratedFunctionBase<T> {
    using OutputContainer = C;
    OutputContainer *output;
    AddContainedFun(OutputContainer& l) : output(&l) {}
    bool Invoke(Object const& q)
    {
        if (!q.Valid()) {
            return true;
        }
        StorageBase* base = q.GetStorageBase(q.GetHandle());
        if (base != nullptr) {
            output->push_back(*base);
        }
        return true;
    }
};

KAI_END
