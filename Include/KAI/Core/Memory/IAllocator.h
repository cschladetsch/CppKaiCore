#pragma once

#include <KAI/Core/Config/Base.h>
#include <KAI/Core/Debug.h>

#include <concepts>
#include <memory>
// Using variant instead of expected which may not be available yet
#include <optional>
#include <span>
#include <system_error>
#include <variant>

KAI_BEGIN

namespace memory
{
/// interface for all memory allocation systems
struct IAllocator {
   protected:
       IAllocator() = default;
       virtual ~IAllocator() = default;

   public:
       using VoidPtr = void*;
       using BytePtr = char*;

       using size_t = std::size_t;

       using Allocator = VoidPtr (*)(size_t);
       using DeAllocator = void (*)(VoidPtr, size_t);

       virtual VoidPtr AllocateBytes(size_t) = 0;
       virtual void DeAllocateBytes(VoidPtr, size_t) = 0;

       template <class T> void Construct(T* ptr)
       {
           ::new (ptr) T;
       }

       template <class T, class U> void Construct(T* ptr, U const& v)
       {
           ::new (ptr) T(v);
       }

    template <class T>
    void Destruct(T *ptr) {
        if (ptr) {
            ptr->~T();
        }
    }

    template <typename T>
        requires std::default_initializable<T>
    std::optional<T *> Allocate() {
        size_t numBytes = sizeof(T);
        VoidPtr bytes = AllocateBytes(numBytes);
        if (!bytes) {
            return std::nullopt;
        }

        T *ptr = reinterpret_cast<T *>(bytes);
        try {
            Construct(ptr);
        } catch (...) {
            DeAllocateBytes(bytes, numBytes);
            return std::nullopt;
        }
        return ptr;
    }

    template <typename T, typename U>
        requires std::constructible_from<T, U>
    std::optional<T *> Allocate(U const &val) {
        size_t numBytes = sizeof(T);
        VoidPtr bytes = AllocateBytes(numBytes);
        if (!bytes) {
            return std::nullopt;
        }

        T *ptr = reinterpret_cast<T *>(bytes);
        try {
            Construct(ptr, val);
        } catch (...) {
            DeAllocateBytes(bytes, numBytes);
            return std::nullopt;
        }
        return ptr;
    }

    template <class T>
    void DeAllocate(T *ptr) {
        if (!ptr) {
            return;
        }
        try {
            Destruct(ptr);
        } catch (const std::exception &e) {
            KAI_TRACE_ERROR() << "exception destructing object at " << ptr
                              << ": " << e.what();
        } catch (...) {
            KAI_TRACE_ERROR()
                << "unknown exception destructing object at " << ptr;
        }
        DeAllocateBytes(reinterpret_cast<VoidPtr>(ptr), sizeof(T));
    }

    template <typename T>
        requires std::default_initializable<T>
    std::optional<std::span<T>> AllocateArray(size_t n)
    {
        size_t numBytes = sizeof(T) * n;
        VoidPtr base = AllocateBytes(numBytes);
        if (!base) {
            return std::nullopt;
        }

        T* typedBase = reinterpret_cast<T*>(base);
        try {
            for (size_t i = 0; i < n; ++i) {
                Construct(typedBase + i);
            }
        } catch (...) {
            // Clean up any constructed elements
            for (size_t i = 0; i < n; ++i) {
                try {
                    Destruct(typedBase + i);
                } catch (...) {
                    // Ignore nested exceptions during cleanup
                }
            }
            DeAllocateBytes(base, numBytes);
            return std::nullopt;
        }
        return std::span<T>(typedBase, n);
    }

    template <class T> void DeAllocateArray(T* ptr, size_t n)
    {
        if (!ptr) {
            return;
        }
        auto base = reinterpret_cast<BytePtr>(ptr);
        try {
            for (; n != 0; --n) {
                Destruct(reinterpret_cast<T *>(base));
                base += sizeof(T);
            }
        } catch (const std::exception &e) {
            KAI_TRACE_ERROR()
                << "exception releasing object at " << base << ": " << e.what();
        } catch (...) {
            KAI_TRACE_ERROR()
                << "unknown exception releasing object at " << base;
        }
        DeAllocateBytes(ptr, sizeof(T) * n);
    }
};
} // namespace memory

KAI_END
