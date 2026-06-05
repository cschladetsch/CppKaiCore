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

CommonLang's lexer uses `boost.monotonic` (an in-house, header-only library,
also by the same author: CppMonotonic). When building standalone, point
`KAI_MONOTONIC_INCLUDE_DIR` at those headers.

## Building

Standalone:

    cmake -B build -DKAI_MONOTONIC_INCLUDE_DIR=/path/to/CppMonotonic
    cmake --build build

Or consume it from a parent project with `add_subdirectory(CppKaiCore)`, which
defines the `Core`, `CommonLang` and `Executor` targets with public include
directories.
