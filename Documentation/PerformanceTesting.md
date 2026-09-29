# FMT Performance Testing

How to run, read and extend the performance and allocation benchmarks of FMT.

Why FMT measures performance, and the preallocation rules the benchmarks check, are in
[Architecture.md, Performance and Memory Efficiency](Architecture.md#performance-and-memory-efficiency).
Tests protect behavior; benchmarks show performance effects. Neither replaces the other.

## Where things are

| Path | Contents |
| --- | --- |
| `Tests/Performance/Harness/` | The harness, a static library (`FMTBenchmarkHarness`): timing, allocation monitor, memory, checks, JSON results. It does not depend on the benchmarks. |
| `Tests/Performance/FMTPerformanceTests.cpp` | The benchmark executable, `FMTPerformanceTests`. |
| `Tests/Performance/*Benchmarks.cpp` | The benchmarks, one file per group. |
| `Tests/Performance/performance.csv` | One row per benchmark: its registration with ctest, its expected result and its allocation bound. |
| `Tests/Performance/CompareResults.cmake` | Compares the results of two runs. |
| `Examples/Models/TWD_land/Scenarios/perfyields/` | The data of the complex-yield benchmarks. |

## Running the benchmarks

### Short mode, with the tests

Every row of `performance.csv` is a ctest test labelled `performance`, and also `allocation` when the
row bounds the allocations. With the rest of the suite, the benchmarks run in a short mode: a few calls,
enough to check that each one still builds, computes the expected result and respects its allocation
bound.

```bash
ctest --test-dir build/release -C Release -L performance
ctest --test-dir build/release -C Release -L allocation
```

The times printed in short mode, and in any run made with `-j`, are not measurements.

### Measurement

A measurement runs the same tests in full mode, one at a time, on a Release build:

```bash
FMT_BENCHMARK_MODE=full ctest --test-dir build/release -C Release -L performance
```

In PowerShell, set the variable with `$env:FMT_BENCHMARK_MODE="full"` first. Each benchmark writes its
results to `build/release/tests/performance/<benchmark>.json`.

### The executable

`FMTPerformanceTests` can also run directly, from `build/release/bin/Release`:

| Option | Meaning |
| --- | --- |
| `--benchmark <name>` | Runs one benchmark, named exactly. |
| `--filter <pattern>` | Runs the benchmarks whose name matches; `*` matches any text. Default: `*`. |
| `--mode smoke\|full` | Short run or measurement. Default: `FMT_BENCHMARK_MODE`, else `smoke`. |
| `--model <primary file>` | Project read by the benchmarks. Default: TWD_land in the source tree. |
| `--expectations <csv>` | Expected results and bounds. Default: `Tests/Performance/performance.csv`. |
| `--expected <value>` | Replaces the expected result of the benchmark named by `--benchmark`. |
| `--max-allocations <n>` | Replaces its allocation bound. |
| `--output <json file>` | Results file. Default: `build/release/tests/performance/<selection>.json`. |
| `--list` | Prints the names of the selected benchmarks and runs none. |

```bash
FMTPerformanceTests --mode full --filter "ComplexYield.*"
```

The exit code is 0 when every selected benchmark is valid, 1 when a check fails or when nothing
matches the selection, and 77 when every selected benchmark skipped a check, which ctest reports as
skipped.

## What a benchmark does

A benchmark is a class derived from `Performance::Benchmark`. The runner calls it in phases:

1. **prepare**, once, not measured: read the model, build the objects, reserve the storage;
2. **warm-up**: first calls, which fill lazy initializations and caches;
3. **calibration**: the number of calls per sample doubles until a sample lasts long enough to be far
   above the resolution of the clock;
4. **timed samples**, with the allocation monitor removed, so that counting costs nothing to the
   times: minimum, maximum, median, mean and standard deviation of the duration of one call;
5. **counted calls**, one at a time, with the allocation monitor installed: the allocations of each
   call, and its result.

| Setting | Short mode | Full mode |
| --- | --- | --- |
| Warm-up calls | 3 | 100 |
| Timed samples | 3 | 30 |
| Minimum duration of a sample | 0.1 ms | 5 ms |
| Counted calls | 20 | 1000 |

The counted calls are then checked against the row of the benchmark in `performance.csv`:

- the result of the last call, and of every counted call, must equal the expected result, within a
  relative tolerance of 1e-6;
- a typical call, the median of the counted calls, must not allocate more than the bound. A bound of
  `0` is stricter: no counted call may allocate.

The bound applies to a typical call because some calls legitimately allocate more than others: a value
put in a cache, a container that grows now and then. A bound of `0` states that a path does not allocate
after preparation, which is what [Architecture.md](Architecture.md#preallocate-before-calculation) asks
of calculation paths.

No check applies to the times. Timing thresholds may come once enough reference results exist.

## The allocation monitor

Under MSVC, `operator new` is linked into each module, so replacing it in the executable would not see
the allocations made inside `FMTlib.dll`. Every module ends up calling the heap functions of the C
runtime through its import table: `Performance::AllocationMonitor::install` redirects `malloc`,
`calloc`, `realloc`, `free`, `_aligned_malloc`, `_aligned_realloc` and `_aligned_free` in the import
table of every loaded module, including the solvers and Boost, to functions that count and forward the
call. FMT is not modified, and the measured build is the delivered one.

By default, only the allocations of the thread that runs the benchmark are counted.

Not counted:

- allocations made inside the C runtime itself, such as `strdup` or stdio buffers;
- direct calls to `HeapAlloc` or `VirtualAlloc`;
- modules linked to a static C runtime.

Aligned blocks are counted, but left out of the live bytes. Outside Windows the monitor is not
available: a row with an allocation bound is then reported as skipped.

The counts do not depend on the allocator: when mimalloc redirects `malloc`, the calls still go through
the redirected imports, and the complex-yield benchmarks count the same allocations. Durations and
memory do depend on it, which is why every result records the allocator of its run.

## Results

Each run writes one JSON file: the environment of the run, then one entry per benchmark. The layout is
version 1 of the schema; any change to it changes `schemaVersion` and this section.

| Environment field | Meaning |
| --- | --- |
| `fmtVersion`, `fmtBuildDate` | Version and build date of `FMTlib`. |
| `features` | Optional components of `FMTlib`, as reported by `Version::FMTVersion::hasFeature`. |
| `allocator` | Allocator that serves `malloc` in the process: `CRT heap`, or `mimalloc <version>` when mimalloc redirects the C runtime. |
| `commit`, `dirty` | Commit of the sources when the benchmarks were built, and whether the working tree then differed from it (modified or untracked files). |
| `buildType`, `optimized` | Configuration of the build, and whether it was compiled with `NDEBUG`. |
| `compiler`, `compilerVersion` | Compiler of the benchmarks. |
| `os`, `cpu`, `logicalCores` | Machine. |
| `availableMemoryBytes` | Physical memory available at the start of the run. |
| `timestamp` | Start of the run, in UTC. |
| `mode` | `smoke` or `full`. |

| Result field | Meaning |
| --- | --- |
| `benchmark`, `group`, `dataset`, `threads` | What was measured, on which data, with how many threads. |
| `samples`, `callsPerSample` | Timed samples; their product is the number of timed calls. |
| `minNs`, `maxNs`, `medianNs`, `meanNs`, `stddevNs` | Duration of one call over the samples, in nanoseconds. |
| `allocationCalls` | Counted calls. |
| `allocationsPerCallMin`, `allocationsPerCallMedian`, `allocationsPerCallMax` | Allocations of one counted call. |
| `allocatedBytesPerCallMedian` | Bytes allocated by a typical call. |
| `allocations`, `deallocations`, `allocatedBytes` | Totals over the counted calls. |
| `peakLiveHeapBytes` | Highest amount of memory allocated and not yet freed during the counted calls. |
| `processPeakPrivateBytes` | Peak private memory of the process at the end of the benchmark. |
| `result`, `expected`, `maxAllocationsPerCall` | Result of the last counted call, and the expectation it was checked against. |
| `valid`, `skipped`, `skipReason`, `failures` | Outcome of the checks. |

The allocation fields are `null` where the monitor is not available.

## Comparing two runs

```bash
cmake -DBASELINE=<file or folder> -DCANDIDATE=<file or folder> -P Tests/Performance/CompareResults.cmake
```

A folder stands for every `.json` file it holds, such as `build/release/tests/performance` after a
measurement. For each benchmark, the report gives the change of the median duration, the allocations
and bytes of a typical call before and after, and the change of the process peak memory:

```text
ComplexYield.Sum
  Median duration:  -0.2%  (378.1 ns -> 377.2 ns)
  Allocations:      4 -> 4 per call
  Allocated bytes:  82 -> 82 per call
  Peak memory:      +0.4%  (process peak private bytes)
```

Durations only compare between two measurements of the same mode, on the same machine, in the same
build type and with the same allocator: the report warns when they differ. Two measurements of the same commit differ by a few
percent. The comparison only reports; nothing fails on a change.

To show the effect of a change, measure the commit before it, keep the results out of the build folder,
measure the commit with it, and compare.

## Adding a benchmark

1. **Write the benchmark** in the `*Benchmarks.cpp` file of its group, or in a new one added to
   `Tests/Performance/CMakeLists.txt`. Its name is `<group>.<operation>[.<variant>]`. Everything the
   measured operation needs is built in `prepare`; `run` performs the operation once and returns a
   result that depends on it.
2. **Add it to the suite** in `FMTPerformanceTests.cpp`.
3. **Compute the expected result by hand**, from the files of the model, never from what FMT prints:
   a benchmark that measures a wrong computation must fail.
4. **Add its row** to `performance.csv`: `FMTPerformanceTests;<name>;<expected result>;<bound>`. Leave
   the bound empty for a first run.
5. **Measure the bound**: run the benchmark and read the allocations per call, then write that median
   as the bound. Run it twice: the median must not change.
6. **See each check fail** without recompiling: `--expected` with a wrong value, `--max-allocations`
   below the measure.
7. **Reconfigure CMake**, which registers the new row with ctest. A value changed in an existing row
   needs no reconfiguration: the executable reads the file at run time.

A benchmark reads its data from `Examples/Models` and writes nothing there. A dataset used by a
benchmark stays stable: a new need gets a new scenario, since any change to an existing one changes what
its benchmarks measure. A changed expected result or bound is justified in the commit that changes it.

## Current benchmarks

### Complex yields

`ComplexYield.<operator>` evaluates one complex yield of the `perfyields` scenario of TWD_land through
`Core::FMTYields::get`, for the development `UNITE1 PEUPLEMENT1 UTR1` at age 7:

| Benchmark | Yield | Expected result |
| --- | --- | --- |
| `ComplexYield.Sum` | `_SUM(VOLUMETOTAL,VOLUMETOTAL)` | 240 |
| `ComplexYield.Multiply` | `_MULTIPLY(VOLUMETOTAL,2)` | 240 |
| `ComplexYield.Divide` | `_DIVIDE(VOLUMETOTAL,2)` | 60 |
| `ComplexYield.Subtract` | `_SUBTRACT(VOLUMETOTAL,20)` | 100 |
| `ComplexYield.Shift` | `_SHIFT(VOLUMETOTAL,1)` | 130 |
| `ComplexYield.Equation` | `_EQUATION(VOLUMETOTAL+VOLUMETOTAL)` | 240 |
| `ComplexYield.RecursiveChain` | five nested `_SUM` | 720 |

The yield requests are built before the measurement, as the graph of a model does. Each call asks for the
yield of the next of 255 developments that differ by their period only: the same value under 255 keys of
the yield cache. `FMTComplexYieldHandler::get` puts a value in that cache only when its computation lasted
more than 0.05 ms, which these computations normally do not. A computation slowed down beyond that, by a
preemption for instance, puts its key in the cache for the rest of the process. With 255 keys, such an
event changes one call in 255, and the minimum of the allocations per call shows it; asked under a
single key, the same yield would switch to the cache for good at the first such event.
