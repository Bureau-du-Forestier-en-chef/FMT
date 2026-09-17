# 🧭 FMT Coding Standards

> **Forest Management Tool**
>
> This document defines the coding standards for FMT. It complements [`Architecture.md`](Architecture.md), which describes the architectural layers, responsibilities, dependency rules, and long-term direction of the project.

---

## 📚 Table of Contents

- [Purpose](#-purpose)
- [Scope](#-scope)
- [General Principles](#-general-principles)
- [Language Standard](#-language-standard)
- [Portability](#-portability)
- [File Organization](#-file-organization)
- [Naming](#-naming)
- [Readability and Declaration Order](#-readability-and-declaration-order)
- [Formatting](#-formatting)
- [Includes](#-includes)
- [Namespaces](#-namespaces)
- [Classes and Responsibilities](#-classes-and-responsibilities)
- [Public API Design](#-public-api-design)
- [Documentation](#-documentation)
- [Type Safety](#-type-safety)
- [Ownership and Lifetime](#-ownership-and-lifetime)
- [Error Handling](#-error-handling)
- [Logging and Events](#-logging-and-events)
- [Thread Safety](#-thread-safety)
- [Templates](#-templates)
- [Serialization](#-serialization)
- [Numerical Code](#-numerical-code)
- [Wrappers and Bindings](#-wrappers-and-bindings)
- [Compiler Warnings](#-compiler-warnings)
- [Testing](#-testing)
- [Refactoring](#-refactoring)
- [Backward Compatibility](#-backward-compatibility)
- [Code Review Checklist](#-code-review-checklist)
- [Living Document](#-living-document)

---

## 🎯 Purpose

These standards aim to make FMT code:

- readable;
- maintainable;
- portable;
- testable;
- loosely coupled;
- strongly typed;
- consistent across components;
- safe to evolve across C++, Python, R, .NET, and Excel interfaces.

These rules apply primarily to new code and modified code. Existing legacy code should be improved incrementally when it is touched, without introducing unnecessary unrelated changes.

---

## 🔭 Scope

This document applies to:

- native C++17 code;
- public C++ headers;
- application controllers and services;
- infrastructure components;
- C++/CLI and .NET wrappers;
- Python bindings;
- Rcpp modules;
- Excel integrations;
- CMake build definitions;
- automated tests.

Architecture and dependency rules are defined in:

```text
Documentation/Architecture.md
```

Contribution workflow should be defined separately in:

```text
CONTRIBUTING.md
```

---

## ✨ General Principles

New code and refactoring should favor:

- readability as the primary code-quality objective;
- a natural top-to-bottom reading order;
- clear responsibilities;
- loose coupling;
- explicit dependencies;
- strong typing;
- portability;
- RAII;
- deterministic behavior;
- independent testability;
- backward compatibility;
- the simplest design that satisfies the requirements.

Code should communicate intent clearly without requiring unnecessary comments or knowledge of unrelated components.

> [!IMPORTANT]
> Existing code does not automatically define the preferred standard for new code.

Avoid introducing abstractions only for theoretical flexibility. An abstraction should provide a practical benefit such as clearer responsibilities, improved testability, portability, or support for multiple implementations.

---

## 🧰 Language Standard

FMT uses **C++17**.

New portable code should use standard C++17 facilities when they provide the required behavior.

Prefer standard facilities such as:

```cpp
std::filesystem
std::optional
std::variant
std::string_view
std::unique_ptr
std::function
```

when their use improves clarity and portability.

Do not introduce a dependency on a newer C++ standard unless the supported toolchains and project configuration are intentionally updated.

Compiler-specific extensions may be used only in components that are explicitly platform-specific or when no suitable portable alternative exists.

---

## 🌍 Portability

Portable components should compile with the supported configurations of:

- MSVC on Windows;
- GCC on Linux;
- Clang where supported.

### Platform-specific code

Platform-specific code should be isolated using:

- dedicated source files;
- dedicated wrappers;
- focused interfaces;
- CMake platform conditions;
- small and localized preprocessor sections.

Avoid scattering platform checks throughout domain code:

```cpp
#ifdef _WIN32
```

Prefer dedicated implementations when practical:

```text
EnvironmentWindows.cpp
EnvironmentLinux.cpp
```

### File-system paths

Prefer:

```cpp
std::filesystem::path
```

for portable native file-system operations.

Do not assume:

- Windows path separators;
- drive letters;
- case-insensitive file systems;
- a specific current working directory;
- a fixed maximum path length;
- a platform-specific text encoding.

### Platform types

The portable domain and application layers must not expose:

- `System::String^`;
- Windows handles;
- Qt or GTK objects;
- Python objects;
- Rcpp objects;
- Excel-specific types.

Wrappers must convert platform-specific values into portable C++ types before calling portable services.

---

## 📁 File Organization

### One primary class per file

Each non-template class should generally have one header and one implementation file:

```text
FMTAction.h
FMTAction.cpp
```

A file should normally contain one primary class.

Small private implementation helpers may remain in a `.cpp` file when they are used only by that implementation and have no meaning outside it.

### Template classes

Template classes and their implementations should generally remain in one `.hpp` file:

```text
FMTBounds.hpp
```

This ensures that template definitions remain visible at the point of instantiation.

### Header and source responsibilities

Headers should contain:

- declarations;
- public API documentation;
- required inline definitions;
- template definitions;
- serialization templates when required.

Source files should contain:

- non-template implementations;
- private implementation details;
- platform-specific implementation code;
- internal helper functions.

### Header guards

All headers must use a consistent include guard or the project-approved alternative.

Example:

```cpp
#ifndef FMTACTION_H_INCLUDED
#define FMTACTION_H_INCLUDED

// Declaration

#endif
```

---

## 🏷️ Naming

Naming should make the code read naturally and communicate intent without requiring the reader to inspect the implementation first.

Names should be chosen according to both meaning and scope.

### Classes and structures

Use PascalCase and preserve the established FMT prefix for public native FMT types:

```cpp
class FMTModel;
class FMTAction;
struct FMTEvent;
```

Class names should describe the responsibility or domain concept represented by the class.

Avoid generic names such as:

```cpp
Manager
Helper
Processor
Data
```

unless the surrounding namespace and responsibility make the meaning unambiguous.

### Public methods

Public method names should be **short, clear, and expressive**.

They should describe the operation from the caller's perspective without exposing implementation details.

Prefer:

```cpp
load()
save()
plan()
validate()
getOutput()
setParameter()
```

Avoid unnecessarily long names such as:

```cpp
performCompleteModelValidationAndReturnResult()
```

Public names should remain concise, but not vague. A short name is useful only when its meaning is clear in the context of the class.

For example:

```cpp
model.validate();
parser.readProject();
controller.clearScenarios();
```

Public methods must not begin with an underscore.

### Private methods

Private method names should be explicit about their internal responsibility and must begin with a single underscore:

```cpp
_loadModel()
_validateScenario()
_rebuildCache()
_publishEvent()
```

A private method may use a longer name than a public method when the additional detail improves maintainability.

The underscore communicates that the method is an implementation detail and is not part of the public API.

Do not use double underscores. Names containing double underscores are reserved by the C++ implementation.

Avoid:

```cpp
__loadModel()
```

Prefer:

```cpp
_loadModel()
```

### Local variables

Local variables should use specific and descriptive names that explain their immediate role.

Prefer:

```cpp
scenarioIndex
selectedOutput
minimumPeriod
convertedPath
```

Avoid vague names:

```cpp
value
data
temp
obj
```

unless the variable is extremely short-lived and its meaning is immediately obvious.

A local variable should normally be understandable without reading several surrounding functions.

### Variables with a large scope

Variables visible across a larger scope should use short, stable names that represent well-established concepts in that scope.

The name should remain concise because it will appear frequently throughout the component, but it must still be meaningful.

Examples include:

```cpp
model
cache
logger
solver
context
```

Do not use cryptic abbreviations merely to shorten a name.

Avoid:

```cpp
mdl
cch
slv
ctx2
```

A large-scope variable should only have a short name when the concept is central and unambiguous throughout that scope.

### Parameters

Parameter names should describe what the caller supplies.

Follow the established convention of the component, but do not mix styles in the same new interface.

Avoid mixing:

```cpp
modelIndex
p_modelIndex
lmodelindex
```

Prefer one consistent form within the component.

### Data members

Use a consistent convention within each component.

For new components, the `m_` prefix is preferred for private data members:

```cpp
m_eventHandler
m_modelCache
m_outputPath
```

Data-member names should identify the stored concept rather than its implementation type.

Prefer:

```cpp
m_models
```

rather than:

```cpp
m_modelVector
```

unless the container type itself is important to the design.

### Constants

Use names that clearly communicate immutability and meaning. Follow the convention of the surrounding component.

Avoid unexplained numeric literals. Use named constants when the value has domain or algorithmic meaning.

---

## 📖 Readability and Declaration Order

Readability is the primary coding-style objective in FMT.

A class should read naturally from top to bottom, almost like prose. The reader should first understand what the class offers, then how it maintains its state, and finally how it implements its internal behavior.

### Public interface first

Declare the public interface first.

The preferred class order is:

1. public constructors and destructor;
2. public operations in a logical usage order;
3. private data members;
4. private helper methods.

Example:

```cpp
class FMTModelService
{
public:
    FMTModelService();
    ~FMTModelService() = default;

    void load(const std::filesystem::path& path);
    bool validate() const;
    void save(const std::filesystem::path& path) const;

private:
    std::vector<FMTModel> m_models;
    bool m_loaded = false;

    void _clearInvalidModels();
    void _validateState() const;
};
```

This order allows a reader to understand the class contract before seeing implementation details.

### Methods should read like prose

Public methods should be ordered according to the normal workflow of the class.

For example:

```text
construct
configure
load
validate
execute
query results
save
close
```

Related overloads should remain together.

Getters and setters should be grouped with the capability they support rather than placed automatically at the beginning or end of every class.

Avoid alphabetical ordering when it makes the usage flow harder to understand.

### No protected members

Do not declare protected data members or protected methods in new code.

Avoid:

```cpp
class Base
{
protected:
    std::vector<FMTModel> models;
    void rebuildCache();
};
```

Protected members expose implementation details to derived classes and create tight coupling between the base class and its inheritance hierarchy.

Prefer private members with a focused public or private abstraction:

```cpp
class Base
{
public:
    const std::vector<FMTModel>& models() const;

private:
    std::vector<FMTModel> m_models;

    void _rebuildCache();
};
```

When a derived class requires customizable behavior, prefer one of the following:

- composition;
- a focused strategy object;
- a public non-virtual interface delegating to a private virtual implementation;
- an explicit protected-free interface between collaborators.

Existing protected members may be preserved for backward compatibility, but new code should not expand their use. Refactoring should reduce protected state incrementally when tests and compatibility constraints permit.

### Keep declarations close to their purpose

Methods that form a coherent feature should be declared next to each other.

For example:

```cpp
void setEventHandler(EventHandler handler);
void clearEventHandler();
bool hasEventHandler() const;
```

Do not separate closely related declarations across distant parts of a class.

### Implementation order

Definitions in the `.cpp` file should follow the same order as declarations in the header.

This makes navigation predictable and allows the reader to move through the interface and implementation in the same sequence.

### Optimize for the reader

Code is read more often than it is written.

Prefer a structure that lets a new contributor understand the common path quickly, even when another order might save a few lines or match historical placement.

### Keep methods short

Methods should be short, focused, and readable from top to bottom without requiring the reader to track several unrelated responsibilities at once.

A method should normally perform one clear operation at one level of abstraction.

Prefer extracting focused private methods over creating one large method containing validation, conversion, orchestration, computation, logging, and result formatting.

Avoid:

```cpp
void executePlanning()
{
    // Validate input.
    // Load the model.
    // Configure the solver.
    // Build constraints.
    // Execute planning.
    // Format outputs.
    // Write files.
    // Publish events.
}
```

Prefer:

```cpp
void executePlanning()
{
    _validateRequest();
    _loadModel();
    _configureSolver();
    _buildConstraints();
    _solve();
    _writeOutputs();
    _publishCompletion();
}
```

It is better to have several short, well-named private methods than one giant method.

Short methods should:

- express one clear intent;
- remain at a consistent level of abstraction;
- avoid deeply nested control flow;
- make failure paths easy to identify;
- be independently understandable;
- make unit testing and refactoring easier.

Private extraction is encouraged when a block of code:

- performs a distinct step of a workflow;
- requires its own meaningful name;
- contains nested conditions or loops;
- is repeated;
- hides the main path of the public method;
- can be tested or reasoned about separately.

Do not extract trivial methods merely to reduce line count. A private method should improve readability by naming a meaningful operation.

### Prefer focused classes over giant classes

When a class accumulates many unrelated private methods, this may indicate that it owns too many responsibilities.

Prefer several focused classes and services over one giant class that coordinates and implements every operation.

A large workflow may be decomposed into collaborating classes such as:

```text
Planning
Selection
Rasterization
ModelQuery
OutputWriter
```

The goal is not to maximize the number of methods or classes. The goal is to create small, cohesive units with clear responsibilities and explicit dependencies.

A public method should present a concise use case. Its private methods should explain the implementation as a sequence of meaningful steps. If those steps represent separate responsibilities, move them into focused collaborator classes.


---

## 🎨 Formatting

FMT source code should be formatted consistently.

If a root `.clang-format` file exists, it is the authoritative mechanical formatting configuration.

Code must use normal spacing and indentation. Do not compress multiple declarations, statements, or method implementations onto one line.

Avoid:

```cpp
if (value) { run(); return true; }
```

Prefer:

```cpp
if (value)
{
    run();
    return true;
}
```

Use blank lines to separate logical sections, but avoid excessive vertical spacing.

Keep function signatures readable. Long declarations should be split across lines:

```cpp
FMTYldBounds::FMTYldBounds(
    FMTsection section,
    FMTkwor key,
    const std::string& yieldName,
    const double& upper,
    const double& lower)
```

Formatting-only changes should not be mixed with unrelated behavioral changes when that would make review more difficult.

---

## 📥 Includes

### Include what is used

Do not rely on transitive includes.

A file using `std::vector` should include:

```cpp
#include <vector>
```

### Include order

Use the following general order:

1. corresponding header;
2. other FMT project headers;
3. third-party headers;
4. standard-library headers.

Example:

```cpp
#include "FMTAction.h"

#include "FMTModel.h"
#include "FMTTransition.h"

#include <boost/serialization/nvp.hpp>

#include <string>
#include <vector>
```

### Forward declarations

Use forward declarations when they reduce coupling and are valid for the required use.

Do not use a forward declaration when the full definition is required for:

- inheritance;
- member objects stored by value;
- template instantiation requiring a complete type;
- inline operations accessing the type.

### Public headers

Public headers should expose the minimum required dependencies.

Platform-specific and wrapper-specific headers must not leak unnecessarily into portable public headers.

---

## 📦 Namespaces

Place code in the namespace matching its responsibility.

Example:

```cpp
namespace Core
{
class FMTAction;
}
```

C++17 nested namespace syntax may be used where it improves clarity:

```cpp
namespace FMTWrapper::Core
{
class Controller;
}
```

Avoid `using namespace` directives in header files.

In source files, prefer explicit namespace qualification when it improves clarity and avoids collisions.

Namespaces should represent stable concepts rather than temporary implementation details.

---

## 🧱 Classes and Responsibilities

A class should have one clear primary responsibility.

- Domain behavior belongs in domain classes.
- Workflow orchestration belongs in application services.
- Wrappers convert types and delegate operations.
- Controllers coordinate use cases.
- Infrastructure components communicate with external technologies.

A class should not gain unrelated responsibilities simply because it is already widely used.

### Wrappers

Wrapper classes such as `FMTForm` should primarily:

- convert managed or language-specific types;
- translate exceptions at interoperability boundaries;
- forward typed events;
- delegate operations to portable controllers or services.

Wrappers should not implement:

- forest-planning business rules;
- solver algorithms;
- rasterization algorithms;
- duplicated selection logic;
- domain behavior already available in native components.

### Controllers

Controllers may:

- validate application-level requests;
- obtain required models and services;
- coordinate a use case;
- publish typed events;
- return application-level results.

Controllers should not become God objects containing unrelated workflows and domain algorithms.

Complex operations should be delegated to focused services.

---

## 🔐 Public API Design

Public APIs should be:

- intentional;
- stable;
- strongly typed;
- portable where applicable;
- documented from the user's perspective;
- consistent with the surrounding interface.

Avoid exposing implementation details through public interfaces.

Prefer domain types or enums instead of unrelated integers when the type carries meaningful constraints.

Public methods should avoid unnecessary output parameters when a return type can express the result clearly.

Changes to public native C++ APIs must be evaluated for their effects on:

- Python;
- R;
- .NET;
- Excel;
- existing native applications.

---

## 📖 Documentation

### Documentation scope

Only public classes and public methods accessible to FMT library users require public API documentation.

This includes symbols exposed through one or more supported interfaces:

- C++;
- Python;
- R;
- .NET;
- Excel.

Internal classes, private methods, protected implementation details, and local helper functions do not require public API documentation.

They should only be commented when necessary to explain:

- a non-obvious algorithm;
- an important invariant;
- a compatibility constraint;
- a compiler or platform workaround;
- a performance-related decision;
- a lifetime or ownership constraint;
- behavior outside the current component.

### Public documentation content

Public documentation should describe:

- the purpose of the class or method;
- observable behavior;
- input parameters;
- return values;
- exceptions and error conditions;
- ownership or lifetime requirements;
- thread-safety guarantees where relevant;
- important side effects;
- compatibility or deprecation information.

Example:

```cpp
/**
 * @brief Returns the number of planning periods in the model.
 *
 * @param modelIndex Index of the model in the model cache.
 * @return Number of planning periods.
 *
 * @throws Exception::FMTException If the model index is invalid.
 */
int getPeriodCount(int modelIndex) const;
```

### Comment quality

Comments should explain intent or reasoning rather than repeat the code.

Avoid:

```cpp
// Increment the index.
++index;
```

Prefer:

```cpp
// Period zero represents the initial inventory and is excluded from
// the planning horizon.
++index;
```

Public documentation should describe behavior from the library user's perspective and should not expose unnecessary implementation details.

---

## 🧷 Type Safety

Prefer strongly typed interfaces.

Avoid untyped mechanisms such as:

```cpp
void*
```

when the interaction can be represented with:

- a concrete type;
- an enum class;
- a template;
- a focused interface;
- `std::function`;
- a typed event.

### Container sizes and indexes

Use:

```cpp
std::size_t
```

for container sizes and indexes where appropriate.

Do not implicitly convert `std::size_t` to `int`.

If an API requires `int`, validate the range and make the conversion explicit.

### Narrowing conversions

Avoid implicit narrowing conversions such as:

```text
std::size_t to int
double to float
unsigned integer to floating-point type
```

When narrowing is intentional:

1. validate the value when data loss is possible;
2. use an explicit cast;
3. document the reason if it is not obvious.

Example:

```cpp
if (value > static_cast<std::size_t>(std::numeric_limits<int>::max()))
{
    throw std::overflow_error("Value does not fit in int");
}

const int converted = static_cast<int>(value);
```

### Boolean expressions

Do not use relational operators on Boolean values unless the behavior is intentional and clearly justified.

Prefer explicit logical expressions:

```cpp
if (!sourceEnabled || targetEnabled)
```

rather than:

```cpp
if (sourceEnabled <= targetEnabled)
```

---

## 🧠 Ownership and Lifetime

Use RAII for resource management.

Prefer automatic storage whenever practical.

Use:

- references for required non-owning dependencies;
- pointers for optional non-owning dependencies;
- `std::unique_ptr` for exclusive dynamic ownership;
- `std::shared_ptr` only when ownership is genuinely shared;
- `std::weak_ptr` to observe shared objects without extending their lifetime.

A raw pointer does not automatically imply ownership.

Do not replace raw pointers mechanically with smart pointers without understanding the ownership model.

Resources such as files, locks, solver handles, and external-library objects should be released automatically through RAII.

Callbacks and event handlers must not outlive the object they invoke.

---

## 🚨 Error Handling

Use the established FMT exception hierarchy for native failures.

Do not silently ignore failures.

Exceptions should contain enough context to identify:

- the failed operation;
- the relevant model, file, or scenario where appropriate;
- the source component;
- actionable diagnostic information.

Catch exceptions at the layer that can meaningfully handle or translate them.

Catch-all handlers such as:

```cpp
catch (...)
```

should normally be limited to application and interoperability boundaries where the exception is immediately translated, logged, or rethrown.

Do not use exceptions for ordinary control flow.

Functions should leave objects in a valid state when an exception occurs.

---

## 📣 Logging and Events

Domain classes should not depend on UI-specific logging or event mechanisms.

When a controller has one event consumer, prefer a typed callback:

```cpp
std::function<void(const Event&)>
```

rather than an untyped callback context.

Example:

```cpp
using EventHandler = std::function<void(const Event&)>;
```

Event values should use portable native types.

Conversion to .NET, Python, R, Qt, GTK, or another platform belongs in the wrapper or application layer.

Event handling must define clear lifetime and thread-safety behavior.

Do not invoke an event handler after the object owning the handler has been destroyed.

If multiple simultaneous consumers become necessary, the typed event model may evolve into a dispatcher without changing domain behavior.

---

## 🧵 Thread Safety

Thread-safety guarantees must be explicit for components used concurrently.

Do not assume that the following are thread-safe:

- model instances;
- solvers;
- caches;
- loggers;
- event handlers;
- serializers;
- external-library handles.

Shared mutable state should be minimized.

Synchronization should be owned by the component responsible for the protected resource.

Avoid exposing internal mutexes or requiring callers to know internal locking rules.

Tests involving concurrency should avoid fragile timing assumptions.

---

## 🧩 Templates

Template definitions should generally remain in `.hpp` files.

Template code must follow the same standards for:

- readability;
- indentation;
- documentation of public APIs;
- type safety;
- exception safety;
- portability.

Avoid overly generic templates when a concrete domain type would communicate intent more clearly.

Template error messages should remain understandable where practical. Use constraints expressed through C++17 techniques only when they materially improve correctness.

---

## 💾 Serialization

Serialization changes require special care because existing user data may depend on the current representation.

Before modifying serialized classes:

- determine whether previously generated files must remain readable;
- preserve serialized field names where required;
- update versions when needed;
- add compatibility tests;
- review all supported archive formats.

A structural refactoring must not silently invalidate existing serialized data.

Serialization functions should include all required state and avoid serializing temporary caches unless that behavior is intentional.

---

## 🔢 Numerical Code

Forest-planning and optimization results may be sensitive to apparently minor implementation changes.

Refactoring numerical code should avoid unintentionally changing:

- floating-point precision;
- iteration order;
- solver coefficients;
- constraint ordering;
- schedule ordering;
- random-number generation;
- deterministic behavior.

Use explicit tolerances for floating-point tests.

Avoid direct equality comparisons on computed floating-point values unless exact equality is guaranteed by the operation.

When converting between `double` and `float`, make the conversion explicit and verify that the precision loss is acceptable.

Randomized algorithms should support deterministic tests through controlled seeds where practical.

---

## 🔌 Wrappers and Bindings

Wrappers are interoperability adapters, not alternative implementations of FMT behavior.

### Native conversion

Convert wrapper-specific types before calling portable code.

Example:

```cpp
const std::string filename = Conversions::toStdString(managedFilename);
Controller::load(filename);
```

### Exception translation

Translate native exceptions only at the interoperability boundary.

Do not duplicate the native exception hierarchy inside each wrapper unless required by the target ecosystem.

### Python

Changes to public Python bindings should consider:

- generated `.pyi` files;
- `py.typed` packaging;
- mypy or equivalent validation;
- IDE completion;
- wheel contents.

### R

Rcpp module changes should preserve runtime reflection and IDE completion where practical.

### .NET and Excel

Managed wrappers should not expose portable components to managed types internally. Conversion should occur at the boundary.

Each shared library should use an export macro appropriate to that library. Import and export macros must not be active simultaneously for the same library implementation.

---

## ⚠️ Compiler Warnings

New code should not introduce warnings in supported build configurations.

Warnings involving potential data loss, unsafe expressions, inconsistent linkage, or portability must be investigated rather than disabled globally.

Examples include:

```text
C4244  Potential data loss during conversion
C4267  Conversion from std::size_t to a smaller type
C4273  Inconsistent DLL linkage
C4804  Unsafe Boolean operation
```

Warnings issued from standard-library or third-party headers should be traced back to the FMT call site before being considered external noise.

Warning suppression must be:

- localized;
- justified;
- documented;
- limited to the smallest possible scope.

Do not lower the project-wide warning level to hide a local problem.

---

## 🧪 Testing

FMT intends to increase unit-test coverage substantially and rely heavily on automated tests for new features and refactoring.

### New features

New observable behavior should include tests for:

- expected use;
- invalid input;
- boundary conditions;
- relevant failure modes.

### Bug fixes

A bug fix should include a regression test that fails before the correction and passes afterward.

### Refactoring

Refactoring should rely on tests to demonstrate that observable behavior remains unchanged.

When existing behavior lacks coverage, add characterization tests before or during the refactoring.

### Test levels

Use the appropriate test level:

- unit tests for domain behavior;
- service tests for application workflows;
- integration tests for solvers, GDAL, ONNX, serialization, and file formats;
- wrapper tests for type conversion and public API exposure;
- compatibility tests for existing formats and interfaces.

### Test independence

Important native behavior should be testable without requiring:

- `FMTForm`;
- a graphical interface;
- Python;
- R;
- Excel;
- manual interaction;
- unrelated global state.

### Determinism

Tests should not depend on:

- execution order;
- uncontrolled random values;
- unstable external services;
- machine-specific paths;
- fragile timing assumptions.

### CMake and CTest

Tests should be integrated with CMake and registered with CTest where practical.

A developer should be able to build and run the relevant tests consistently from the command line and continuous integration.

### Coverage

Coverage should identify important untested paths and risky components.

Coverage percentage is a diagnostic indicator, not the objective by itself. Meaningful assertions and representative scenarios matter more than executing lines without validating behavior.

---

## ♻️ Refactoring

Refactoring should improve internal structure without unintentionally changing observable behavior.

Primary refactoring objectives include:

- clearer responsibilities;
- reduced coupling;
- improved portability;
- improved testability;
- reduced duplication;
- stronger typing;
- explicit ownership;
- simpler maintenance.

### Preserve behavior

Observable behavior includes:

- public API signatures;
- generated outputs;
- optimization results;
- exception types;
- logging and event behavior;
- file and serialization formats;
- Python, R, .NET, Excel, and C++ interfaces.

If behavior changes intentionally, document and test the work as a feature or bug fix rather than hiding it inside a structural refactoring.

### Small and reviewable changes

Prefer focused changes over broad refactoring that combines:

- file movement;
- renaming;
- behavioral changes;
- new features;
- formatting;
- dependency upgrades.

Separate concerns into distinct commits or pull requests when practical.

### Duplication

Equivalent logic should have one authoritative implementation.

Compatibility methods may delegate to the authoritative implementation but should not maintain duplicated behavior.

### Dead code

Remove dead and commented-out code once supported interfaces and tests confirm it is no longer required.

Version control preserves history. Source files should not be used as archives of obsolete implementations.

### Documentation

Update `Architecture.md` when a refactoring changes:

- component responsibilities;
- dependency direction;
- architectural boundaries;
- project-wide design principles.

---

## 🛡️ Backward Compatibility

FMT exposes public functionality across several ecosystems.

Before renaming, moving, or removing a public symbol, verify whether it is accessible through:

- C++;
- Python;
- R;
- .NET;
- Excel.

When practical:

- preserve an existing API through a compatibility alias;
- delegate the old API to the new implementation;
- document deprecation;
- provide a migration path;
- add compatibility tests.

Do not keep two independent implementations of old and new behavior.

Changes to file formats, serialization, numerical behavior, or generated output require the same compatibility review as public API changes.

---

## ✅ Code Review Checklist

Before submitting or approving a change, consider the following.

### Architecture

- [ ] Is the code placed in the correct architectural layer?
- [ ] Are responsibilities clear and focused?
- [ ] Does the change favor loose coupling?
- [ ] Are dependencies explicit?
- [ ] Does portable code remain free of platform-specific types?

### API

- [ ] Are public API changes intentional?
- [ ] Were Python, R, .NET, Excel, and C++ impacts considered?
- [ ] Are public classes and methods documented?
- [ ] Is backward compatibility preserved where practical?

### Safety

- [ ] Is ownership clear?
- [ ] Are narrowing conversions explicit and safe?
- [ ] Are exception and lifetime guarantees reasonable?
- [ ] Are event-handler lifetime and thread-safety concerns handled?

### Quality

- [ ] Can each class be read naturally from top to bottom?
- [ ] Are methods short and focused on one operation?
- [ ] Was a giant method decomposed into meaningful private methods?
- [ ] Does each class remain cohesive rather than accumulating unrelated responsibilities?
- [ ] Is the public interface declared before private implementation details?
- [ ] Do `.cpp` definitions follow the declaration order in the header?
- [ ] Are new protected members avoided?
- [ ] Do private methods begin with a single underscore?
- [ ] Are public method names short, clear, and caller-focused?
- [ ] Are local and large-scope variable names appropriate to their scope?
- [ ] Is duplicated behavior avoided?
- [ ] Is the code formatted and readable?
- [ ] Are includes explicit and minimal?
- [ ] Are there new compiler warnings?
- [ ] Is dead or commented-out code removed?

### Tests

- [ ] Do new features include tests?
- [ ] Do bug fixes include regression tests?
- [ ] Is refactored behavior protected by tests?
- [ ] Are tests deterministic and focused?
- [ ] Can the relevant behavior be tested without a UI or wrapper?

---

## 📝 Living Document

These coding standards should evolve with FMT.

When the project adopts a new repository-wide convention, this document should be updated as part of the same change.

A convention should be enforceable, understandable, and beneficial. Rules that no longer serve the project should be revised rather than preserved only by habit.
