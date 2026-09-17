---
name: Bug report
about: Report a defect or unexpected behavior in FMT
title: "[BUG] "
labels: bug
assignees: ''
---

# 🐛 Bug Report

## 📋 Summary

Provide a short and clear description of the issue.

> Example:
>
> Strategic planning crashes when an output uses a level constraint and MOSEK is selected as the solver.

---

# 🎯 What Happened?

Describe the observed behavior.

- What did you do?
- What did FMT do?
- Why does this appear incorrect?

---

# ✅ Expected Behavior

Describe what you expected FMT to do.

---

# 🔁 Steps To Reproduce

Provide the smallest reproducible workflow possible.

1.
2.
3.
4.

---

# 📦 FMT Version

- FMT Version:
- Commit SHA (if built from source):
- Branch:
- Build Date:

---

# 🖥 Environment

## Operating System

- [ ] Windows
- [ ] Linux
- [ ] macOS

Version:

---

## Compiler

- [ ] MSVC
- [ ] GCC
- [ ] Clang

Version:

---

## Build Type

- [ ] Debug
- [ ] Release

---

## Interface Used

- [ ] C++
- [ ] Python
- [ ] R
- [ ] .NET

Version (if applicable):

---

## Solver

- [ ] CLP
- [ ] CBC
- [ ] Gurobi
- [ ] MOSEK
- [ ] Other

Solver version:

---

# 📖 Minimal Reproducible Example

## C++

```cpp
// Minimal reproducer
```

## Python

```python
# Minimal reproducer
```

## R

```r
# Minimal reproducer
```

## Woodstock / PRI Fragment

```text
Paste the smallest model fragment reproducing the issue.
```

---

# 📂 Input Data

Does this issue depend on specific files?

- [ ] No
- [ ] Yes

If yes, please attach or provide:

- PRI file
- Yield file
- Transition file
- Shape file
- Raster data
- Schedule file
- Other supporting inputs

---

# 💥 Error Message

```text
Paste the complete error message here.
```

---

# 📚 Stack Trace

```text
Paste the complete stack trace here.
```

---

# 📸 Screenshots / Logs

Include screenshots, logs, console output, or generated reports if relevant.

---

# 🔄 Reproducibility

How consistently does the issue occur?

- [ ] Always
- [ ] Frequently
- [ ] Sometimes
- [ ] Rarely
- [ ] Unable to reproduce

---

# 📈 Impact

How severe is the issue?

- [ ] Cosmetic
- [ ] Minor inconvenience
- [ ] Workflow blocking
- [ ] Model produces incorrect results
- [ ] Data corruption
- [ ] Crash
- [ ] Security concern

---

# 🧪 Workaround

Have you found a workaround?

- [ ] No
- [ ] Yes

If yes, describe it:

---

# 📎 Additional Context

Add any additional information that may help diagnose the issue:

- Related commits
- Related issues
- Performance observations
- Solver logs
- Output examples
- Regression information

---

# ✔ Checklist

Before submitting:

- [ ] I searched existing issues and discussions.
- [ ] I included steps to reproduce the problem.
- [ ] I attached all relevant files or provided a minimal reproducible example.
- [ ] I included the complete error message and stack trace.
- [ ] I verified the issue on the latest available version when possible.
