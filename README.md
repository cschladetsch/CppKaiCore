# CppKaiCore

The foundational runtime of the KAI system, extracted from
[CppKAI](https://github.com/cschladetsch/CppKAI) as a reusable module
(including for Android).

It bundles three mutually-dependent libraries:

- **Core** - object model, registry, reflection, type system, memory and
  containers.
- **Executor** - the continuation-based virtual machine.
- **CommonLang** - the shared lexer/parser/translator base used by the KAI
  languages.

These form a dependency cycle, so they ship together as one module.

## Dependencies

None beyond a C++23 standard library. CommonLang's lexer can optionally use
arena allocation via `std::pmr` (enable `-DKAI_USE_MONOTONIC_ALLOCATOR`);
otherwise it uses plain `std::vector` / `std::map`.

## Building

Standalone:

    cmake -B build
    cmake --build build

Or consume it from a parent project with `add_subdirectory(CppKaiCore)`, which
defines the `Core`, `CommonLang` and `Executor` targets with public include
directories.
