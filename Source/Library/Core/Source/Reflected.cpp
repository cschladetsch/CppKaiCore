#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Object.h>

KAI_BEGIN

Registry &Reflected::Reg() const {
    if (!self || !self->Exists()) KAI_THROW_0(NullObject);

    return *self->GetRegistry();
}

KAI_END
