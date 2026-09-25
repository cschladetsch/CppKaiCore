#pragma once

#include <KAI/Core/Config/Base.h>

#include "Constness.h"
#include "Handle.h"

KAI_BEGIN

struct ObjectConstructParams {
    const ClassBase* classBase;
    Registry *registry;
    Handle handle;
    Constness constness;

    ObjectConstructParams() : classBase(nullptr), registry(nullptr) {}
    ObjectConstructParams(Registry *, const ClassBase *, Handle,
                          Constness = Constness::Mutable);
    ObjectConstructParams(const Object &, Constness);
    ObjectConstructParams(const StorageBase *);
    ObjectConstructParams(StorageBase *);
};

KAI_END
