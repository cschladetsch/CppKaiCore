#pragma once

#include <KAI/Core/BasePointerBase.h>
#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Type.h>

KAI_BEGIN

template <class T>
struct BasePointer : BasePointerBase, FwdBasePointer<T> {
    using Base = T;
    using BaseTraits = typename Type::Traits<Base>;
    std::shared_ptr<Base> base;

    BasePointer(Base* p = 0) : base(p) {}

    [[nodiscard]] Base* GetBase() const
    {
        return &*base;
    }
    Base *operator->() const { return GetBase(); }
    Base &operator*() const { return *GetBase(); }

    static void Register(Registry& r)
    {
        KAI_UNUSED(r);

        /* MUSTFIX
        ClassBuilder<BasePointer<T> >(R, String("BasePointer"))
            ;
            */
    }
};

KAI_TYPE_TRAITS(BasePointerBase, Number::BasePointer,
                Properties::StringStreamInsert);

KAI_END
