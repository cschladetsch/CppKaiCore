---
layout: default
title: CppKaiCore
---

# CppKaiCore

Core library for KAI: a network-transparent object/runtime system with two
cooperating languages, Pi and Rho, built on a tri-colour garbage collector
and a Registry/Domain object model.

## Architecture

- **Pi** — a stack-based (RPN) language and the underlying evaluator.
- **Rho** — an infix, Python-like language that transpiles to Pi. Duck-typed
  today; static typing is a planned future addition.
- **Tau** — an IDL for generating network-transparent Agent/Proxy pairs
  from C++ types.
- **Core** — the object model: Registries (collections of addressable
  Executors) grouped into Domains, tri-colour GC, and a three-part
  `node:reg:#` addressing scheme. Executors can migrate between Domains
  without breaking existing references.
- **Executor** — the Pi/Rho evaluator and console/command layer.
- **Language** — the Pi and Rho language front ends (parsing, execution).

See [Architecture](architecture) for diagrams of the language pipeline,
object model, and cross-node consensus.

## Layout

- `Include/KAI/` — public headers (`Core`, `Executor`, `Language`, `Console`).
- `Source/Library/` — implementation, mirroring the `Include` layout
  (`Core`, `Executor`, `Language`).

## Design notes

- No global state anywhere in the design.
- Parallelism is intended to come from running multiple Registries that
  communicate over Tau/Rho/Pi, rather than from threading within a single
  Registry.
- Networking uses ENet; object identity/consensus across nodes is handled
  via a State Update Packet (SUP) model with per-node-pair propagation
  rates, rather than a single global synchronization scheme.

## Status

Extracted as a history-preserving submodule (via `git-filter-repo`) from
the broader KAI project. Core test suite covers Pi, Rho, and the
networking/proxy layers.
