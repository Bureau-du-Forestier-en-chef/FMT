# 🤖 FMT Agent Guidelines

> Instructions for AI coding assistants and automated contributors working in the FMT repository.

---

## 📚 Required Reading

Before modifying the repository, read:

- [`Documentation/Architecture.md`](Documentation/Architecture.md)
- [`Documentation/CodingStandards.md`](Documentation/CodingStandards.md)
- [`CONTRIBUTING.md`](CONTRIBUTING.md), when available

These documents are authoritative. This file is a concise entry point and must not replace them.

---

## 🎯 Project Context

FMT, the **Forest Management Tool**, is a mature C++17 library for forest-planning models.

FMT exposes functionality through multiple interfaces:

- native C++;
- Python;
- R;
- .NET and C++/CLI;
- Excel.

A change to a public native C++ symbol may therefore affect several wrappers, bindings, generated files, packages, and user applications.

FMT is modular and contains identifiable architectural layers, but it is not yet a strictly layered system. New features and refactoring should move the codebase incrementally toward clearer boundaries without requiring a complete rewrite.

---

## 🏗️ Architectural Direction

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

Apply these rules:

1. Domain code must not depend on wrappers or user interfaces.
2. Wrappers convert platform-specific types and delegate operations.
3. Controllers coordinate use cases but should not contain domain algorithms.
4. Complex workflows belong in focused application services.
5. Infrastructure-specific behavior should remain isolated.
6. Platform-specific types must not cross into portable domain interfaces.
7. New dependencies should be explicit and strongly typed.
8. Circular dependencies should be avoided.
9. Shared behavior should have one authoritative implementation.
10. Legacy code does not automatically define the preferred design for new code.

---

## 🔗 Loose Coupling

Loose coupling is a core FMT design philosophy.

New features and refactoring should:

- minimize knowledge between components;
- make dependencies explicit;
- avoid unnecessary global mutable state;
- avoid expanding singleton usage;
- avoid coupling portable code to a specific UI or language binding;
- improve independent testability;
- use focused services and portable data types;
- prefer the simplest useful abstraction.

Do not create an interface, factory, observer, or dependency-injection mechanism only for theoretical flexibility.

Introduce an abstraction when it provides a practical benefit such as:

- clearer responsibilities;
- improved testability;
- portability;
- meaningful dependency isolation;
- support for multiple implementations.

---

## 🌍 Portability

FMT portable code is expected to support the project-approved configurations of:

- MSVC on Windows;
- GCC on Linux;
- Clang where supported.

FMT uses C++17.

When writing portable code:

- prefer standard C++17 facilities;
- avoid unnecessary compiler extensions;
- isolate platform-specific code;
- use `std::filesystem::path` for native file-system paths when practical;
- do not assume Windows separators, drive letters, or case-insensitive file systems;
- do not expose managed, Python, Rcpp, Excel, Qt, GTK, or operating-system types through portable interfaces;
- do not introduce new compiler warnings.

Platform-specific behavior should be contained in dedicated wrappers, adapters, infrastructure components, source files, or localized CMake conditions.

---

## 📁 Files and Classes

Follow these file-organization rules:

- one primary class per file;
- regular classes use a `.h` declaration and `.cpp` implementation;
- template classes and template definitions generally use one `.hpp` file;
- private implementation helpers may remain in a `.cpp` file when they have no meaning outside that implementation;
- public headers should expose the minimum required dependencies;
- do not rely on transitive includes;
- do not compress declarations or implementations onto single lines.

Examples:

```text
FMTAction.h
FMTAction.cpp
FMTBounds.hpp
```

Do not create extra files or abstractions unless they improve the structure meaningfully.

---

## 🎨 Code Style

Follow the root `.clang-format` file when present.

General expectations:

- use readable indentation and spacing;
- place braces consistently with the surrounding component and formatter;
- split long signatures across multiple lines;
- use descriptive names;
- avoid unexplained abbreviations;
- avoid unrelated formatting changes;
- do not add `using namespace` directives to header files;
- include the corresponding header first in a `.cpp` file;
- preserve the established naming convention of the component being modified.

Do not perform a repository-wide naming migration as part of an unrelated change.

---

## 📖 Documentation

Only public classes and public methods accessible to FMT library users require public API documentation.

This includes APIs exposed through:

- C++;
- Python;
- R;
- .NET;
- Excel.

Public documentation should explain:

- purpose;
- observable behavior;
- parameters;
- return values;
- exceptions;
- important side effects;
- ownership and lifetime requirements;
- thread-safety guarantees where relevant;
- compatibility or deprecation information.

Private methods, internal classes, protected implementation details, and local helpers do not require public API documentation.

Add internal comments only when they explain non-obvious intent, invariants, algorithms, workarounds, performance decisions, compatibility constraints, or lifetime requirements.

Do not add comments that merely repeat the code.

---

## 🧷 Type Safety

Prefer strongly typed interfaces.

Avoid introducing:

```cpp
void*
```

when the intent can be represented with:

- a concrete type;
- an `enum class`;
- a focused interface;
- a template;
- `std::function`;
- a typed event.

Use `std::size_t` for container sizes and indexes where appropriate.

Do not implicitly narrow:

- `std::size_t` to `int`;
- `double` to `float`;
- large unsigned integers to smaller or floating-point types.

When narrowing is required, validate the range when necessary and make the conversion explicit.

Do not use relational operators on Boolean values when an explicit logical expression communicates the intent more clearly.

---

## 🧠 Ownership and Lifetime

Use RAII.

Prefer:

- automatic storage;
- references for required non-owning dependencies;
- pointers for optional non-owning dependencies;
- `std::unique_ptr` for exclusive dynamic ownership;
- `std::shared_ptr` only for genuinely shared ownership;
- `std::weak_ptr` for non-owning observation of shared objects.

Do not infer ownership from a raw pointer without examining how the object is created, stored, and destroyed.

Event handlers and callbacks must not outlive the objects they invoke.

---

## 🚨 Exceptions, Logging, and Events

Use the established FMT exception hierarchy for native failures.

Catch exceptions only where they can be handled, translated, or enriched meaningfully.

Restrict `catch (...)` primarily to application and interoperability boundaries where the exception is immediately translated, logged, or rethrown.

Domain code must not depend on UI-specific logging or event mechanisms.

For a controller with one event consumer, prefer:

```cpp
std::function<void(const Event&)>
```

instead of an untyped callback context.

Event payloads should use portable native C++ types. Platform-specific conversion belongs in the wrapper layer.

If multiple simultaneous consumers become necessary, the typed event model may later evolve into an event dispatcher.

---

## 🧪 Testing Requirements

Automated tests are a primary safety mechanism for FMT development and refactoring.

Apply these rules:

1. New observable features require tests.
2. Bug fixes require regression tests when practical.
3. Refactoring must rely on tests to preserve observable behavior.
4. Add characterization tests when existing behavior is insufficiently covered.
5. Domain behavior should be tested without requiring a UI or language wrapper.
6. Controllers and application services should be independently testable.
7. Wrapper tests should focus on conversion, exception translation, event forwarding, and public API availability.
8. Tests must be deterministic and focused.
9. Prefer the smallest practical model and dataset.
10. Register tests with CMake and CTest where practical.

Coverage should identify important untested paths, but coverage percentage is not a substitute for meaningful assertions.

Do not claim that tests passed unless they were actually executed.

When execution is unavailable, clearly state which validation was not performed.

---

## 🔌 Wrappers and Bindings

Wrappers are interoperability adapters, not alternate implementations of FMT behavior.

### Python

When public Python bindings change, consider:

- generated `.pyi` files;
- `py.typed` packaging;
- stub normalization;
- mypy or equivalent validation;
- IDE completion;
- wheel contents.

### R

When Rcpp modules change, preserve runtime reflection and RStudio completion where practical.

### .NET and Excel

Keep managed types at the interoperability boundary.

Convert managed values before calling portable native C++ services.

Each shared library should use an export macro appropriate to that library. Do not define import and export states simultaneously for the same implementation target.

---

## ⚠️ Compiler Warnings

Treat compiler warnings as actionable until investigated.

Pay special attention to:

```text
C4244  Potential data loss during conversion
C4267  Conversion from std::size_t to a smaller type
C4273  Inconsistent DLL linkage
C4804  Unsafe Boolean operation
```

Warnings emitted from standard-library or third-party headers may still originate from an FMT call site.

Do not suppress warnings globally to hide a local problem.

Any unavoidable suppression must be localized, justified, and documented.

---

## ♻️ Refactoring

Refactoring should improve structure without unintentionally changing observable behavior.

Observable behavior includes:

- public API signatures;
- generated outputs;
- numerical and optimization results;
- exception behavior;
- logging and events;
- serialization and file formats;
- C++, Python, R, .NET, and Excel interfaces.

Prefer focused, reviewable changes.

Avoid combining all of the following in one change unless necessary:

- large file movement;
- broad renaming;
- new features;
- behavioral changes;
- dependency upgrades;
- unrelated formatting.

Remove dead and commented-out code after verifying that supported interfaces no longer require it. Version control preserves history.

Update `Documentation/Architecture.md` when responsibilities, dependency direction, or architectural principles change.

---

## 🛡️ Backward Compatibility

Before changing a public symbol, check its exposure through:

- C++;
- Python;
- R;
- .NET;
- Excel.

When practical:

- preserve compatibility through a delegating alias;
- avoid duplicated old and new implementations;
- document deprecation;
- provide a migration path;
- add compatibility tests.

File formats, serialized data, generated outputs, and numerical behavior require the same compatibility care as public APIs.

---

## 🚫 Do Not

Do not:

- place domain logic in a wrapper;
- make the domain depend on a UI or language binding;
- introduce new `void*` callback contexts when a typed alternative exists;
- expand global mutable state without strong justification;
- copy a legacy pattern only because it already exists;
- add public API documentation to every private helper;
- invent test results;
- silently change public behavior during refactoring;
- ignore compiler warnings without investigation;
- modify generated files without updating the generation process;
- perform broad unrelated cleanup in a focused bug fix;
- introduce an abstraction with no practical benefit.

---

## ✅ Before Completing a Change

Verify the following where applicable:

- [ ] The code belongs in the correct architectural layer.
- [ ] Responsibilities remain focused.
- [ ] Dependencies are explicit and loosely coupled.
- [ ] Portable code remains platform-independent.
- [ ] Public API impacts were assessed across supported interfaces.
- [ ] Public user-facing classes and methods are documented.
- [ ] Ownership and lifetime are clear.
- [ ] Narrowing conversions are explicit and safe.
- [ ] No new compiler warnings were introduced.
- [ ] New behavior has tests.
- [ ] Bug fixes have regression tests when practical.
- [ ] Refactored behavior is protected by tests.
- [ ] Generated bindings, stubs, or packages were updated when needed.
- [ ] Architecture or coding documentation was updated when project-wide rules changed.
- [ ] The final response accurately reports what was and was not validated.

---

## 📝 Keeping This File Current

`AGENTS.md` is a concise operational guide.

The authoritative sources remain:

```text
Documentation/Architecture.md
Documentation/CodingStandards.md
```

When those documents change, update this file if the new rule affects how automated contributors should work.
