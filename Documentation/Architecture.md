# 🏗️ FMT Architecture

> **Forest Management Tool**
>
> This document describes the architecture of FMT, the responsibilities of its major components, the intended direction of dependencies, and the principles that guide new development and refactoring.

---

## 📚 Table of Contents

- [Purpose](#-purpose)
- [Architectural Status](#-architectural-status)
- [Architectural Goals](#-architectural-goals)
- [High-Level Architecture](#-high-level-architecture)
- [Layer Responsibilities](#-layer-responsibilities)
- [Dependency Rules](#-dependency-rules)
- [Loose Coupling](#-loose-coupling)
- [Portability](#-portability)
- [Performance and Memory Efficiency](#-performance-and-memory-efficiency)
- [Public Interfaces](#-public-interfaces)
- [Error Handling and Logging](#-error-handling-and-logging)
- [Events and Notifications](#-events-and-notifications)
- [State and Ownership](#-state-and-ownership)
- [Thread Safety](#-thread-safety)
- [Testing as an Architectural Foundation](#-testing-as-an-architectural-foundation)
- [Backward Compatibility](#-backward-compatibility)
- [Known Architectural Debt](#-known-architectural-debt)
- [Direction for New Development](#-direction-for-new-development)
- [Architectural Decision Principle](#-architectural-decision-principle)

---

## 🎯 Purpose

This document provides a shared architectural reference for FMT contributors, maintainers, reviewers, and coding assistants.

It explains:

- how FMT is organized;
- where new code should be placed;
- how components should depend on each other;
- which responsibilities belong to each architectural layer;
- which principles should guide new features and refactoring;
- how FMT intends to evolve without requiring a complete rewrite.

This document describes architectural intent. Detailed naming, formatting, documentation, and coding conventions belong in `Documentation/CodingStandards.md`.

---

## 🧭 Architectural Status

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

---

## ✨ Architectural Goals

The FMT architecture aims to:

- preserve the independence of forest-planning domain logic;
- move progressively toward clearer architectural layers;
- keep wrappers lightweight and focused on interoperability;
- separate workflow orchestration from domain behavior;
- isolate platform-specific and infrastructure-specific implementations;
- enforce portability across supported operating systems and compilers;
- preserve speed as a primary architectural objective;
- reduce memory consumption and memory fragmentation;
- favor preallocation and reuse in calculation-intensive code;
- avoid repeated small allocations and deallocations during calculations;
- use loose coupling as a guiding principle for new features and refactoring;
- make dependencies explicit and strongly typed;
- reduce duplicated behavior across wrappers and interfaces;
- significantly increase unit-test coverage;
- rely heavily on automated tests to support safe development and refactoring;
- preserve backward compatibility whenever practical;
- improve maintainability without introducing unnecessary abstractions.

A concise summary of this direction is:

> **New features and refactoring should favor loose coupling, explicit dependencies, portability, and independent testability. Automated tests are the primary safety mechanism used to preserve FMT behavior as the architecture evolves.**

---

## 🗺️ High-Level Architecture

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

---

## 🧱 Layer Responsibilities

### 🖥️ Applications and User Interfaces

This layer provides user-facing applications and workflows.

Examples include:

- graphical interfaces;
- command-line tools;
- Excel integration;
- future Linux applications;
- future web or service-based applications.

This layer may depend on public wrapper or application interfaces. It should not directly implement forest-planning rules.

---

### 🔌 Wrappers and Language Bindings

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

---

### 🎛️ Application Controllers and Services

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

---

### 🌲 Forest-Planning Domain

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

---

### 🧰 Infrastructure and External Libraries

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

---

## ➡️ Dependency Rules

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

---

## 🔗 Loose Coupling

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

---

## 🌍 Portability

Portability is a core architectural objective of FMT.

FMT is intended to support multiple operating systems, compilers, architectures, and programming-language interfaces.

The primary portability targets include:

- Windows with MSVC;
- Linux with GCC;
- Clang where supported;
- native C++17 applications;
- Python;
- R;
- .NET and Excel where supported.

Portable domain and application components should remain independent from:

- operating-system APIs;
- compiler-specific extensions;
- user-interface frameworks;
- managed runtimes;
- language-binding types.

Platform-specific behavior should be isolated in:

- dedicated wrappers;
- adapters;
- dedicated source files;
- infrastructure implementations;
- localized CMake platform conditions.

Adding support for another platform should not require modifications to unrelated forest-planning domain logic.

Some existing components are intentionally platform-specific. These constraints should be clearly identified and contained rather than spread across unrelated layers.

---

## ⚡ Performance and Memory Efficiency

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

Calculation paths should avoid:

```cpp
new
std::make_unique
std::make_shared
```

They should also avoid hidden allocation caused by:

```cpp
std::vector::push_back
std::vector::resize
std::string concatenation
std::map insertion
std::unordered_map insertion
returning large containers by value
```

These operations are not prohibited everywhere in FMT. They should be avoided inside performance-critical loops and calculation paths unless storage has already been reserved and the operation is known not to increase capacity or allocate memory.

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

---

## 📦 Public Interfaces

FMT exposes functionality through:

- native C++;
- Python;
- R;
- .NET;
- Excel.

The native C++ domain and application APIs form the foundation of these interfaces.

Language bindings and wrappers should expose equivalent concepts when practical, but each interface may adapt naming and types to the conventions of its ecosystem.

Changes to public C++ classes may affect:

- Python bindings and type stubs;
- Rcpp modules;
- .NET and C++/CLI wrappers;
- Excel integrations;
- existing native applications.

Public API changes must therefore be evaluated across all supported interfaces.

Only public classes and public methods accessible to FMT library users require public API documentation. Internal classes, private methods, protected implementation details, and local helpers should only be documented when necessary to explain non-obvious behavior or maintenance constraints.

---

## 🚨 Error Handling and Logging

FMT uses shared exception and logging concepts across several interfaces.

Domain and application components should report failures through native FMT exceptions.

Wrappers are responsible for translating those exceptions into the error-reporting mechanism expected by their target environment.

Logging and feedback mechanisms should remain independent from specific user interfaces.

New cross-layer communication should use strongly typed interfaces. Untyped callback contexts such as:

```cpp
void*
```

should be avoided when the interaction can be represented using a concrete type or typed event handler.

Catch-all exception handlers should normally remain at application or interoperability boundaries where exceptions are immediately translated, reported, or rethrown.

---

## 📣 Events and Notifications

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

---

## 🧠 State and Ownership

Ownership and lifetime should be explicit.

New components should prefer:

- automatic storage and RAII;
- references for required non-owning dependencies;
- pointers for optional non-owning dependencies;
- `std::unique_ptr` for exclusive dynamic ownership;
- `std::shared_ptr` only when ownership is genuinely shared.

Global mutable state and singleton dependencies should not be expanded without strong justification.

Existing caches and singleton services should be isolated behind focused APIs and reduced incrementally when affected components are refactored.

A raw pointer should not be replaced mechanically with a smart pointer without first determining whether it owns the referenced object.

---

## 🧵 Thread Safety

Components that support concurrent execution must clearly define their thread-safety guarantees.

Shared state, caches, event handlers, loggers, solvers, and model instances must not be assumed to be thread-safe unless explicitly designed and documented as such.

Application services should avoid hidden shared mutable state.

Synchronization should be localized to the component that owns the protected resource.

Event handlers must not be invoked after their owner has been destroyed. Registration, replacement, and lifetime rules must be explicit when asynchronous or concurrent execution is involved.

---

## 🧪 Testing as an Architectural Foundation

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

When existing behavior is not sufficiently covered, characterization tests should be added before or during refactoring. A characterization test records current behavior and provides a baseline for safely improving the internal implementation.

### Testing expectations

1. **New features require tests.**

   New observable behavior should include automated tests for the main success cases, failure cases, and relevant boundaries.

2. **Bug fixes require regression tests.**

   A bug fix should include a test that fails before the correction and passes after it.

3. **Refactoring relies on behavior tests.**

   Refactoring should preserve observable behavior unless the change is intentional, documented, and tested.

4. **Domain behavior should be tested at the domain level.**

   Forest-planning rules should be tested without requiring a user interface or language wrapper.

5. **Application workflows should be independently testable.**

   Controllers and services should be testable without requiring `FMTForm`, Excel, Python, R, or another interface.

6. **Wrappers require focused integration tests.**

   Wrapper tests should verify type conversion, exception translation, event forwarding, and public API availability. They should not duplicate every domain test.

7. **Tests should be deterministic.**

   Tests should not depend on execution order, uncontrolled randomness, unstable external state, or fragile timing assumptions.

8. **Tests should remain focused.**

   The smallest practical model and dataset should be used to demonstrate the behavior under test.

9. **Tests must run automatically.**

   Tests should be integrated with CMake and CTest so they can run consistently in local builds and continuous integration.

10. **Coverage should guide improvement.**

    Code coverage should identify untested behavior and risky components. Coverage percentages are indicators, not substitutes for meaningful assertions and representative scenarios.

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

---

## 🛡️ Backward Compatibility

FMT is used through several programming-language and platform interfaces.

Architectural improvements should preserve established public behavior whenever practical.

Compatibility concerns include:

- public class and method names;
- language-binding symbols;
- model and project file formats;
- serialized data;
- exception behavior;
- generated outputs;
- numerical results;
- logging and event behavior.

When an existing public operation is renamed, a compatibility alias may delegate to the new implementation during a documented transition period.

For example:

```cpp
void Cache_InitialiserModelParser(...)
{
    SetErrorsToWarnings(...);
}
```

Compatibility methods should delegate to the authoritative implementation and must not maintain duplicated behavior.

---

## 🧹 Known Architectural Debt

FMT contains historical design decisions that do not necessarily represent the preferred direction for new development.

Examples may include:

- broad façade classes;
- singleton caches;
- shared mutable state;
- untyped callbacks;
- wrapper logic mixed with application workflows;
- duplicated selection or conversion logic;
- export macros shared across distinct libraries;
- direct infrastructure dependencies in high-level components;
- limited tests in some components.

These areas should be improved incrementally when touched by feature development or focused refactoring.

> [!WARNING]
> New code should not copy an existing pattern solely because that pattern already exists in the repository.

Architectural debt should be documented when it cannot be corrected safely within the scope of the current change.

---

## 🚀 Direction for New Development

New features and refactoring should:

- place behavior in the appropriate architectural layer;
- preserve domain independence;
- favor loose coupling;
- make dependencies explicit;
- use strongly typed interfaces;
- avoid unnecessary global state;
- isolate platform-specific behavior;
- remain portable where the component is intended to be portable;
- optimize calculation-intensive code for speed;
- preallocate and reuse memory in hot paths;
- reduce small allocations, deallocations, and memory fragmentation;
- account for per-thread memory cost in parallel workflows;
- add or improve automated tests;
- preserve supported public interfaces whenever practical;
- avoid duplicating behavior across wrappers;
- remain focused and reviewable.

A refactoring should improve internal structure without hiding behavioral changes.

If behavior changes intentionally, the work should be documented and tested as a feature or bug fix rather than presented only as structural cleanup.

---

## ⚖️ Architectural Decision Principle

The purpose of the FMT architecture is not to maximize the number of abstractions.

An architectural change is valuable when it:

- clarifies responsibilities;
- reduces coupling;
- makes dependencies explicit;
- improves portability;
- improves execution speed where performance matters;
- reduces allocation churn and memory fragmentation;
- keeps multithreaded memory use controlled;
- improves testability;
- removes duplication;
- preserves or improves type safety;
- simplifies maintenance;
- supports existing and future public interfaces.

The simplest design that satisfies these requirements should generally be preferred.

---

## 📝 Living Document

This architecture document is expected to evolve with FMT.

When a change introduces a new architectural boundary, modifies layer responsibilities, or establishes a new project-wide principle, this document should be updated as part of the same change.

Architecture is not only the current directory structure. It is the set of responsibilities, dependencies, and decisions that allow FMT to evolve safely.
