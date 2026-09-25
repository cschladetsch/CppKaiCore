#pragma once

#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Memory/IAllocator.h>

KAI_BEGIN

namespace memory {
/// Commonality for all allocators
struct BaseAllocator : memory::IAllocator {
    typedef char Byte;
    typedef char *BytePtr;

    typename memory::IAllocator::Allocator alloc;
    typename memory::IAllocator::DeAllocator free;

   protected:
    BaseAllocator() : alloc(0), free(0) {}

   public:
    VoidPtr AllocateBytes(size_t N) {
        if (!alloc) return 0;
        return alloc(N);
    }
    void DeAllocateBytes(VoidPtr ptr, size_t num_bytes) {
        if (!free) return;
        free(ptr, num_bytes);
    }
};
}  // namespace memory

KAI_END
