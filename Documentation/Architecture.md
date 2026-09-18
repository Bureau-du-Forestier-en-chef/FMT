# FMT Architecture

> **Forest Management Tool**
>
> This document describes the architecture of FMT, the responsibilities of its major components, the intended direction of dependencies, and the principles that guide new development and refactoring.

## Table of Contents

- [Purpose](#purpose)
- [Architectural Status](#architectural-status)
- [Architectural Goals](#architectural-goals)
- [High-Level Architecture](#high-level-architecture)
- [Layer Responsibilities](#layer-responsibilities)
- [Dependency Rules](#dependency-rules)
- [Loose Coupling](#loose-coupling)
- [Portability](#portability)
- [Performance and Memory Efficiency](#performance-and-memory-efficiency)
- [Public Interfaces](#public-interfaces)
- [Error Handling and Logging](#error-handling-and-logging)
- [Events and Notifications](#events-and-notifications)
- [State and Ownership](#state-and-ownership)
- [Thread Safety](#thread-safety)
- [Testing as an Architectural Foundation](#testing-as-an-architectural-foundation)
- [Backward Compatibility](#backward-compatibility)
- [Known Architectural Debt](#known-architectural-debt)
- [Architectural Decision Principle](#architectural-decision-principle)

## Purpose

This document provides a shared architectural reference for FMT contributors, maintainers, reviewers, and coding assistants.

It explains:

- how FMT is organized;
- where new code should be placed;
- how components should depend on each other;
- which responsibilities belong to each architectural layer;
- which principles should guide new features and refactoring;
- how FMT intends to evolve without requiring a complete rewrite.

This document describes architectural intent. Naming, formatting, documentation, and coding conventions belong in [CodingStandards.md](CodingStandards.md). The repository map, build and test mechanics, and the conventions specific to this repository belong in [AGENTS.md](../AGENTS.md).

Each rule lives in one document. Where another document already states a rule, this one links to it rather than restating it, and the rule is updated where it is defined.

This document evolves with FMT. When a change introduces an architectural boundary, modifies layer responsibilities, or establishes a project-wide principle, update it in the same change.

## Architectural Status

FMT is a mature and modular C++17 library with identifiable domain, application, infrastructure, and interoperability layers.

FMT is **not currently a strictly layered system**. Some historical components combine multiple responsibilities or communicate directly across architectural boundaries.

Examples of existing architectural debt may include:

- wrapper classes containing workflow logic;
- shared caches and singleton dependencies;
- logging and exception-handling mechanisms crossing multiple layers;
- direct access to infrastructure components from higher-level modules;
- platform-specific behavior appearing in otherwise portable components;
- duplicated behavior across C++, Python, R, .NET, and Excel interfaces;
- broad façade classes exposing several unrelated operations;
- untyped callbacks and hidden dependencies.

These characteristics reflect the evolution of a mature library supporting several platforms, solvers, workflows, and programming languages.

The objective is to move **incrementally** toward clearer architectural boundaries while preserving existing functionality and backward compatibility whenever practical.

> [!IMPORTANT]
> Existing code does not automatically define the preferred architecture for new code.

## Architectural Goals

The FMT architecture aims to:

- preserve the independence of forest-planning domain logic;
- move progressively toward clearer architectural layers, keeping wrappers focused on interoperability and workflow orchestration out of domain behavior;
- isolate platform-specific and infrastructure-specific implementations;
- enforce portability across supported operating systems and compilers;
- preserve execution speed, and keep memory use and allocation churn controlled in calculation-intensive code;
- use loose coupling as a guiding principle, with explicit and strongly typed dependencies;
- reduce duplicated behavior across wrappers and interfaces;
- rely on automated tests to support safe development and refactoring, and increase unit-test coverage substantially;
- preserve backward compatibility whenever practical;
- improve maintainability without introducing unnecessary abstractions.

A concise summary of this direction is:

> **New features and refactoring should favor loose coupling, explicit dependencies, portability, and independent testability. Automated tests are the primary safety mechanism used to preserve FMT behavior as the architecture evolves.**

## High-Level Architecture

The intended dependency direction is:

```text
Applications and User Interfaces
                ↓
Wrappers and Language Bindings
                ↓
Application Controllers and Services
                ↓
Forest-Planning Domain
                ↓
Infrastructure and External Libraries
```

Dependencies should generally flow downward toward the domain and infrastructure capabilities required by the use case.

The domain must not depend on user interfaces, managed wrappers, or language-binding types.

### Target flow example

```text
.NET, Python, R, C++, Excel, or future Linux UI
                        ↓
             Wrapper or public adapter
                        ↓
          Controller and application service
                        ↓
               FMT domain objects
                        ↓
       Solver, GDAL, ONNX, files, serialization
```

## Layer Responsibilities

Each layer maps to directories and namespaces in the repository:

| Layer | Where it lives |
| --- | --- |
| Applications and user interfaces | `UI/`, `Excel/FMTExcel/`, `Examples/C++/` |
| Wrappers and language bindings | `UI/` (`FMTForm`), `Excel/`, the `Python` and `R` namespaces in `Include/` and `Source/` |
| Application controllers and services | `FMTWrapperCore/` (`Controller`, the `UseCases` classes, `SES`, `Planning`, `TransformationCore`) |
| Forest-planning domain | `Include/` and `Source/`, namespaces `Core`, `Models`, `Spatial`, `Graph`, `Heuristics` |
| Infrastructure and external libraries | `Include/` and `Source/`, namespaces `Parser`, `Logging`, `Exception`, `Parallel`, plus solver, GDAL, and ONNX Runtime integration |

The mapping is not one directory per layer. `Include/` and `Source/` hold several layers separated only by namespace, which is one reason the boundaries have to be maintained deliberately rather than relied upon from the directory structure.

### Applications and User Interfaces

This layer provides user-facing applications and workflows.

Examples include:

- graphical interfaces;
- command-line tools;
- Excel integration;
- future Linux applications;
- future web or service-based applications.

This layer may depend on public wrapper or application interfaces. It should not directly implement forest-planning rules.

### Wrappers and Language Bindings

This layer exposes FMT to supported platforms and programming languages.

Examples include:

- Python bindings;
- Rcpp modules;
- .NET and C++/CLI wrappers;
- Excel integration.

Wrappers are responsible for:

- converting platform-specific types to portable native C++ types;
- converting native results to platform-specific representations;
- translating exceptions across interoperability boundaries;
- forwarding typed events to the target environment;
- delegating application operations to controllers or services.

Wrappers must not duplicate:

- forest-planning business rules;
- solver behavior;
- selection logic;
- planning workflows;
- rasterization algorithms;
- domain validation already implemented elsewhere.

For example, `FMTForm` should act as a .NET interoperability adapter:

```text
System::String^
      ↓ conversion
std::string
      ↓
Controller or application service
```

### Application Controllers and Services

The application layer coordinates use cases and workflows.

Examples may include:

- planning;
- rasterization;
- model queries;
- output and constraint selection;
- operating-area scheduling;
- area variability;
- environment configuration.

Controllers provide application-facing façades. They may:

- validate application-level input;
- retrieve required models or services;
- coordinate use cases;
- invoke specialized application services;
- translate service results into application results;
- publish typed events.

Controllers should remain focused on orchestration. They should not become collections of unrelated algorithms.

Complex workflows should be delegated to specialized services such as:

```text
Planning
Rasterization
Selection
ModelQuery
OperatingArea
AreaVariability
Environment
```

In `FMTWrapperCore` this layer has two levels. `Controller` is the façade the wrappers call: it receives data transfer objects and scenario indexes, delegates each system operation, and holds no logic and no FMT type of its own. The `UseCases` classes behind it resolve the scenario indexes in `ModelCache`, coordinate the workflow, update the session state, and call the services listed above.

### Forest-Planning Domain

The domain layer contains the principal concepts, rules, and behavior of FMT.

Examples include:

- models;
- actions;
- constraints;
- transitions;
- developments;
- schedules;
- outputs;
- yields;
- specifications;
- themes;
- masks.

The domain layer should contain forest-planning behavior and invariants.

It must remain independent from:

- graphical interfaces;
- C++/CLI and managed types;
- Python objects;
- Rcpp objects;
- Excel APIs;
- platform-specific user-interface frameworks.

### Infrastructure and External Libraries

The infrastructure layer provides technical capabilities required by the domain and application layers.

Examples include:

- optimization solvers;
- GDAL and geospatial processing;
- ONNX Runtime;
- file-system access;
- serialization;
- logging implementations;
- operating-system integration;
- external model and data formats.

Infrastructure-specific details should be isolated behind dedicated components or interfaces whenever doing so improves:

- portability;
- testability;
- replaceability;
- clarity of dependencies.

## Dependency Rules

The following rules guide new development:

1. **The domain must not depend on wrappers or user interfaces.**

2. **Wrappers may depend on application services and public native types.**

3. **Application services may coordinate domain and infrastructure components.**

4. **Platform-specific types must not cross into portable domain interfaces.**

5. **Infrastructure details should not leak through public domain APIs unless they are intentionally part of the FMT abstraction.**

6. **New dependencies should be explicit through constructors, method parameters, typed handlers, or focused interfaces.**

7. **Circular dependencies should be avoided.**

8. **Shared behavior should have one authoritative implementation.**

9. **Wrapper-specific functionality must remain in the wrapper layer.**

10. **Legacy dependencies do not automatically define the preferred architecture for new code.**

FMT does not invert these dependencies today: domain components call the solver, GDAL, and serialization services directly rather than through interfaces they define themselves. That remains acceptable. Introduce an interface owned by the caller when it buys testability or replaceability for a specific component, not as a general conversion of the codebase.

## Loose Coupling

Loose coupling is a fundamental design philosophy of FMT.

New features and refactoring should minimize the knowledge that components have about each other. Components should depend on clearly defined behavior rather than unnecessary implementation details.

Loose coupling should make it possible to:

- change an implementation without modifying unrelated components;
- test components independently;
- replace platform-specific implementations;
- support multiple user interfaces and language bindings;
- isolate external dependencies;
- reduce the effect of changes across the codebase;
- reuse domain and application logic outside its original caller.

Dependencies should normally be expressed through:

- constructor parameters;
- method parameters;
- focused interfaces;
- typed event handlers;
- dedicated services;
- portable data-transfer types.

New code should avoid unnecessary reliance on:

- global mutable state;
- hidden initialization order;
- singleton access from unrelated components;
- concrete user-interface types;
- language-binding types;
- untyped callback contexts such as `void*`;
- broad classes that expose unrelated responsibilities.

Loose coupling does not mean that every class requires an interface, factory, or dependency-injection framework.

Abstractions should be introduced when they:

- clarify responsibilities;
- improve testability;
- isolate a meaningful dependency;
- support more than one implementation;
- prevent platform-specific behavior from leaking into portable code.

The simplest design that maintains clear boundaries and explicit dependencies should generally be preferred.

## Portability

Portability is a core architectural objective of FMT. The primary targets are Windows with MSVC, Linux with GCC, and Clang where supported, reached through native C++17, Python, R, .NET, and Excel.

Architecturally this comes down to containment: platform-specific behavior belongs in dedicated wrappers, adapters, source files, infrastructure implementations, or localized CMake conditions, so that supporting another platform does not require touching unrelated forest-planning logic.

Some components are intentionally platform-specific. Those constraints should be identified and contained rather than spread across layers.

The coding rules that follow from this, including path handling and the platform types that must not appear in portable headers, are in [CodingStandards.md, Portability](CodingStandards.md#portability).

## Performance and Memory Efficiency

Execution speed is a primary architectural objective of FMT.

FMT performs calculation-intensive forest-planning, optimization, simulation, graph, yield, raster, and scheduling operations. Performance optimization has historically been an important part of the library and must remain an important part of future development and refactoring.

Memory efficiency is equally important.

FMT performs substantial multithreaded work. Repeated small allocations and deallocations can increase synchronization costs, reduce cache locality, fragment memory, and multiply memory consumption across worker threads.

New calculation-intensive code should therefore be designed for:

- predictable execution cost;
- efficient data access;
- good cache locality;
- bounded temporary storage;
- reuse of existing objects and buffers;
- minimal allocator activity;
- controlled memory use under multithreading;
- avoidance of unnecessary copies;
- reduced memory fragmentation.

### Preallocate before calculation

Objects, strategies, buffers, indexes, source references, temporary arrays, and other calculation resources should be allocated and initialized before the calculation phase whenever their required size can be determined or safely bounded.

During calculation, code should reuse existing storage rather than repeatedly creating and destroying small objects.

The rule concerns capacity, not a list of forbidden functions: inside a calculation path, no operation should grow storage or reach the allocator. Reserve the capacity during the preparation phase, then reuse it.

In practice, watch for the following inside calculation paths:

```cpp
new
std::make_unique
std::make_shared
std::vector::push_back on a vector whose capacity was not reserved
std::vector::resize
std::string concatenation
std::map and std::unordered_map insertion
```

None of these is prohibited in FMT generally. Outside hot paths they are ordinary C++, and a `push_back` into a reserved vector allocates nothing. Returning a container by value is also fine: copy elision and move semantics make it free in the common case.

### Reuse storage

Prefer reusable storage owned by the component responsible for the calculation.

Examples include:

- pre-sized vectors;
- reserved containers;
- reusable scratch buffers;
- stable index-based lookup tables;
- object pools when their complexity is justified;
- contiguous storage;
- spans, views, iterators, or references into existing storage;
- precomputed relationships and indexes.

A calculation may overwrite or reset existing storage, but should not increase its capacity unexpectedly.

If the required capacity depends on model content, determine and reserve it during parsing, model initialization, graph construction, or another preparation phase.

### Avoid memory fragmentation

Repeated allocation and deallocation of many small objects should be treated as an architectural concern, particularly in long-running and multithreaded workflows.

Prefer:

- contiguous collections over individually allocated elements;
- value storage over per-element heap allocation when object size permits;
- stable ownership over repeated construction and destruction;
- batched allocation over many small allocations;
- indexed references over temporary associative containers in hot paths;
- reuse over churn.

When dynamic allocation is unavoidable, allocation frequency and lifetime should be considered explicitly.

### Multithreading and memory

Parallel execution can multiply per-thread memory use.

A design that appears acceptable in one thread may become excessively expensive when replicated across many workers.

For multithreaded calculations:

- identify whether temporary storage is shared, thread-local, or task-local;
- avoid false sharing between frequently modified values;
- avoid unnecessary synchronization around allocators or shared containers;
- bound the memory required by each worker;
- preallocate worker contexts before launching calculations;
- reuse each worker's buffers across tasks when safe;
- do not share mutable scratch storage without explicit synchronization;
- avoid allocating temporary objects repeatedly inside parallel loops.

Thread-local storage should not be used automatically. Its lifetime and multiplied memory cost must be understood.

### Measure before and after

Performance-sensitive changes should be supported by measurement when practical.

Relevant measurements may include:

- elapsed time;
- CPU time;
- peak memory usage;
- allocation count;
- allocated bytes;
- cache behavior;
- scaling across thread counts;
- solver and I/O time where relevant.

Optimization should preserve correctness, deterministic behavior where required, and public compatibility.

Tests protect behavior. Benchmarks and profiling demonstrate performance effects. Neither should replace the other.

### Readability remains required

Performance code must remain readable and testable.

An optimization should be documented when its purpose or constraints are not obvious. Comments should explain why a less direct implementation is required and which performance property it protects.

Do not introduce complexity based only on assumptions. Prefer measured and focused optimization while still designing calculation paths to avoid obvious allocation churn.

### Performance design principle

The preferred calculation lifecycle is:

```text
Parse and initialize
        ↓
Determine required capacities
        ↓
Allocate strategies, objects, indexes, and buffers
        ↓
Launch calculation or worker threads
        ↓
Reuse preallocated storage
        ↓
Produce results without repeated allocation churn
```

The key principle is:

> **Allocate and prepare before calculation. Reuse during calculation. Avoid repeated small allocations and deallocations in hot and multithreaded paths.**

## Public Interfaces

FMT exposes functionality through:

- native C++;
- Python;
- R;
- .NET;
- Excel.

The native C++ domain and application APIs form the foundation of these interfaces.

Language bindings and wrappers should expose equivalent concepts when practical, but each interface may adapt naming and types to the conventions of its ecosystem.

A change to a public C++ class reaches Python bindings and type stubs, Rcpp modules, .NET and C++/CLI wrappers, Excel integrations, and existing native applications. Public API changes must therefore be evaluated across every supported interface; the procedure is in [CodingStandards.md, Backward Compatibility](CodingStandards.md#backward-compatibility).

The scope of public API documentation, and the `// DocString:` marker FMT uses to drive Python and R docstring generation, are defined in [CodingStandards.md, Documentation](CodingStandards.md#documentation).

## Error Handling and Logging

FMT uses shared exception and logging concepts across several interfaces.

Domain and application components should report failures through native FMT exceptions.

Wrappers are responsible for translating those exceptions into the error-reporting mechanism expected by their target environment.

Logging and feedback mechanisms should remain independent from specific user interfaces.

New cross-layer communication should use strongly typed interfaces rather than untyped callback contexts.

How exceptions are raised, caught, and translated is defined in [CodingStandards.md, Error Handling](CodingStandards.md#error-handling).

## Events and Notifications

Application controllers may report:

- logs;
- warnings;
- errors;
- progress updates;
- completed workflows;
- generated outputs;
- future rendering or rasterization results.

When there is a single event consumer, the preferred mechanism is a typed event callback:

```cpp
std::function<void(const Event&)>
```

A portable event model may begin with:

```cpp
enum class EventType
{
    Info,
    Warning,
    Error,
    Progress
};

struct Event
{
    EventType type;
    std::string message;
};
```

This approach:

- removes untyped `void*` callbacks;
- keeps controllers independent from `FMTForm` and other interfaces;
- avoids unnecessary inheritance in wrapper classes;
- supports different user interfaces through platform-specific handlers;
- creates a clear path toward a future event dispatcher if multiple simultaneous consumers become necessary.

Event payloads should use portable native C++ types.

Conversion to .NET, Python, R, Qt, GTK, or another platform belongs in the relevant wrapper or application layer.

## State and Ownership

Architecturally, the concern is shared state rather than individual pointers.

Global mutable state and singleton dependencies should not be expanded without strong justification. Existing caches and shared services should be isolated behind focused APIs and reduced incrementally as the components around them are refactored: shared state is what makes a component impossible to test alone, and what makes parallel execution expensive.

The ownership rules that apply when writing a class are in [CodingStandards.md, Ownership and Lifetime](CodingStandards.md#ownership-and-lifetime).

## Thread Safety

A thread-safety guarantee is part of a component's contract: a component used concurrently must state what it guarantees, and nothing in FMT should be assumed thread-safe by default.

FMT runs substantial parallel work, so this is an architectural property and not only an implementation detail. A component that hides shared mutable state cannot be made safe by its callers.

The rules for writing such components are in [CodingStandards.md, Thread Safety](CodingStandards.md#thread-safety).

## Testing as an Architectural Foundation

FMT aims to significantly increase its unit-test coverage and rely more heavily on automated tests when developing new features and refactoring existing components.

Tests are part of the architecture. They provide the safety required to improve a mature library while preserving established behavior across C++, Python, R, .NET, and Excel interfaces.

New features should be designed so that their principal behavior can be tested independently from:

- graphical user interfaces;
- managed runtimes;
- language bindings;
- external applications;
- unrelated global state;
- complete production-scale forest models;
- manual interaction.

Refactoring should rely on tests to demonstrate that observable behavior remains unchanged.

When existing behavior is not sufficiently covered, characterization tests should be added before or during refactoring.

What to test, at which level, and how tests are registered are defined in [CodingStandards.md, Testing](CodingStandards.md#testing) and in [AGENTS.md](../AGENTS.md).

### Relationship between testing and loose coupling

Loose coupling and automated testing reinforce each other.

A component that cannot be tested without constructing a user interface, initializing unrelated global state, loading a full production environment, or configuring an external platform is likely coupled to responsibilities outside its intended role.

When a component is difficult to test, its design should be examined for:

- hidden dependencies;
- excessive responsibilities;
- direct access to global state;
- platform-specific dependencies;
- concrete infrastructure dependencies;
- untyped communication mechanisms;
- tightly coupled object lifetimes.

Testing should be considered during API and component design, not added only after implementation.

## Backward Compatibility

FMT is used through several programming-language and platform interfaces.

Architectural improvements should preserve established public behavior whenever practical. The compatibility surface, and the procedure for changing a public symbol, are in [CodingStandards.md, Backward Compatibility](CodingStandards.md#backward-compatibility).

When an existing public operation is renamed, a compatibility alias may delegate to the new implementation during a documented transition period. The alias holds no behavior of its own:

```cpp
// Deprecated: kept for the Excel and .NET callers, remove after the transition period.
void setErrorsToWarnings(bool p_enabled)
{
    setExceptionHandlerToWarnings(p_enabled);
}
```

Compatibility methods delegate to the authoritative implementation and must not maintain a second copy of the behavior. Two independent implementations of the old and new API is the failure mode this rule exists to prevent.

## Known Architectural Debt

FMT contains historical design decisions that do not represent the preferred direction for new development. A debt list is only useful if it names real code, so the following are concrete and checkable:

| Debt | Where |
| --- | --- |
| Broad façade at the interoperability boundary, 814 lines of declarations | `UI/Include/FMTForm.h` |
| Untyped `const void*` vertex handles crossing a public interface | `Include/FMTGraphVertexToYield.h` |
| Two parameter conventions in the same header, legacy `l` prefix next to `p_` | `Include/FMTModel.h` and others |
| Protected members exposing base-class internals to derived classes | 31 of 247 headers |
| Behavior covered mainly by end-to-end example executables rather than unit tests | `Examples/C++/` |

These should be improved incrementally when a feature or a focused refactoring touches them. Add a row when a change reveals a new case, and remove one when it is closed.

> [!WARNING]
> New code should not copy an existing pattern solely because that pattern already exists in the repository.

Architectural debt should be documented when it cannot be corrected safely within the scope of the current change.

## Architectural Decision Principle

The purpose of the FMT architecture is not to maximize the number of abstractions. The goals listed at the top of this document are the test: an architectural change is worth making when it advances one of them, and among the designs that do, the simplest is the right one.

An abstraction introduced without one of those goals behind it is a cost with no return. Clearer responsibilities, reduced coupling, better testability, or a real portability or performance gain justify an interface, a service, or a new type. Theoretical flexibility does not.

One consequence deserves stating on its own: a refactoring should improve internal structure without hiding behavioral changes. When behavior changes intentionally, document and test the work as a feature or a bug fix rather than presenting it as structural cleanup.
