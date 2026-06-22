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

## Executor Identity and Trees

`Executor` is a reflected Registry object and is identified by its Registry
`Handle`. A Registry may contain multiple live Executors. Each Executor carries
its own `Tree*` attachment through `SetTree()`/`GetTree()`; callers must not infer
an Executor from a Console-global Tree or assume that every Executor shares one
Tree. Inspector and debugger clients should enumerate live Executor objects,
select one by handle, and then read that selected Executor's Tree, root, scope,
data stack, and context stack.

The parent CppKAI Console exposes a bounded machine snapshot for NodeGLM and
handle-targeted debugger actions. Snapshot lifecycle, debugger attachment and
actions, and failures use KAI's `Logger`; snapshot payloads use stdout strictly
as a transport channel.

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
