---
layout: default
title: Architecture
---

# CppKaiCore Architecture

## Language pipeline

Rho source transpiles to Pi, which the Executor evaluates directly. Tau
generates the Agent/Proxy pairs that make C++ types network-transparent.

```mermaid
flowchart LR
    subgraph Languages
        Rho[Rho<br/>infix, Python-like]
        Pi[Pi<br/>stack-based / RPN]
    end
    Tau[Tau<br/>IDL]

    Rho -- transpiles to --> Pi
    Pi --> Executor[Executor<br/>Pi/Rho evaluator]
    Tau -- generates --> Agent[Agent]
    Tau -- generates --> Proxy[Proxy]
    Agent --> Executor
    Proxy --> Executor
```

## Object model: Registry / Domain / Executor

A Domain is a collection of Registries. Each object is addressed by a
three-part `node:reg:#` scheme, and Executors can migrate between Domains
without breaking existing references.

```mermaid
flowchart TB
    subgraph Domain
        subgraph Registry_A[Registry]
            Ex1[Executor #1]
            Ex2[Executor #2]
        end
        subgraph Registry_B[Registry]
            Ex3[Executor #3]
        end
    end

    Ref[Reference<br/>node:reg:#] -.addresses.-> Ex1
    Ex2 -- migrates --> Registry_B
```

## Cross-node object consensus (SUP)

Object state is treated as the reconciled "center of the flock" of every
node holding an opinion on it. State Update Packets (SUPs) are pushed at a
frequency that falls off with distance, using a per-node-pair propagation
rate rather than one global constant. Each object tracks a "world-line": a
time-space frame with time kept relative to local flock time.

```mermaid
sequenceDiagram
    participant Node A
    participant Node B
    participant Node C

    Note over Node A,Node C: Each node holds an opinion on shared object state
    Node A->>Node B: SUP (rate: alpha_AB)
    Node A->>Node C: SUP (rate: alpha_AC, lower - greater distance)
    Node B->>Node C: SUP (rate: alpha_BC)
    Note over Node A,Node C: State converges to the reconciled "center of the flock"
```

## Parallelism model

KAI avoids threading within a single Registry. Parallelism instead comes
from running multiple Registries that communicate over Tau/Rho/Pi.

```mermaid
flowchart LR
    R1[Registry 1] <-- Tau/Rho/Pi --> R2[Registry 2]
    R2 <-- Tau/Rho/Pi --> R3[Registry 3]
    R1 <-- Tau/Rho/Pi --> R3
```
