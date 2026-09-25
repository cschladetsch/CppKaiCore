#pragma once

#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Object/Label.h>

#include <map>

KAI_BEGIN

// TODO: use unordered map
using Dictionary = std::map<Label, Object>;

KAI_END
