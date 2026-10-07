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
| `Tests/Performance/PerfYields.h`, `.cpp` | The `perfyields` scenario and its prepared yield requests, shared by the yield benchmarks. |
| `Tests/Performance/FlowBenchmarks.h`, `.cpp` | The model flows: optimization, replay of a schedule, outputs, simulation, replanning. |
| `Tests/Performance/DefinedBenchmarks.h`, `.cpp` | The benchmarks defined by a row of an expectations file, such as the private ones. |
| `Tests/Performance/QuietFmt.h`, `.cpp` | Makes FMT quiet, once per process (see [Adding a benchmark](#adding-a-benchmark)). |
| `Tests/Performance/performance.csv` | One row per benchmark: its registration with ctest, its expected result and its bounds. |
| `Tests/Performance/performance-private.csv` | A local file that git ignores: the private benchmarks, on models outside the source tree. |
| `Tests/Performance/CompareResults.cmake` | Compares the results of two runs. |
| `Examples/Models/TWD_land/Scenarios/perfyields/` | The data of the complex-yield benchmarks. |

## Running the benchmarks

### Short mode, with the tests

Every row of `performance.csv` is a ctest test labelled `performance`, and also `allocation` when the
row bounds the allocations of a call, and `memory` when it bounds the memory a call keeps or the peak
memory of the process. With the rest of the suite, the benchmarks run in a short mode: a few calls,
enough to check that each one still builds, computes the expected result and respects its bounds.

```bash
ctest --test-dir build/release -C Release -L performance
ctest --test-dir build/release -C Release -L allocation
ctest --test-dir build/release -C Release -L memory
```

The rows of `performance-private.csv` are labelled `bfec-perf` only, and stay out of the base suite
(see [Private benchmarks](#private-benchmarks)):

```bash
ctest --test-dir build/release -C Release -L bfec-perf
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
| `--max-retained-bytes <n>` | Replaces its bound on the memory a call keeps. |
| `--max-peak-memory <MB>` | Replaces its bound on the peak private memory of the process, in megabytes. |
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

| Setting | Short mode | Full mode | Slow operation, short | Slow operation, full | Model flow, short | Model flow, full |
| --- | --- | --- | --- | --- | --- | --- |
| Warm-up calls | 3 | 100 | 1 | 2 | 0 | 1 |
| Timed samples | 3 | 30 | 2 | 10 | 1 | 3 |
| Minimum duration of a sample | 0.1 ms | 5 ms | none: one call | none: one call | none: one call | none: one call |
| Counted calls | 20 | 1000 | 3 | 20 | 1 | 1 |

A slow operation, one that lasts milliseconds or more such as reading a project, says so in
`Benchmark::getSettings`, which then returns `RunSettings::forSlowCalls`: its measurement lasts seconds,
not hours. A model flow, from milliseconds on TWD_land to minutes on a production model, returns
`RunSettings::forFlows`. A sample of one call needs no calibration.

A flow times its phases in the same calls as the whole operation: it declares them with `definePhase`
before its first call, marks the start of a call with `beginPhases`, and the end of each phase with
`endPhase`. The results give the median, minimum and maximum of each phase, so that a change shows where
it costs: in FMT, such as reading or building a model, or in the solver.

Before the first benchmark, the suite asks Windows not to throttle the process, and keeps the thread that
measures on the fastest cores of the processor (`ProcessorPolicy`). A hybrid processor, such as the 13th
generation of Intel Core, mixes performance cores with efficiency cores that run the same code up to 1.7
times slower, and Windows moves a process whose window is in the background to the efficiency cores.
Without this, a benchmark could run on either kind, and its times would jump.

A full measurement then waits 250 ms before its first benchmark. On the Windows 11 machine where the
suite was written, a process that has just read its first file is paused for about 30 ms some 50 ms
later, and runs slower until then, even when it does nothing else; the suite reads `performance.csv`
as it starts. Without the wait, every benchmark had one sample about 30 ms too long, and the median of
a project read was a third too high. The short mode does not wait: its times are not a measurement.

The counted calls are then checked against the row of the benchmark in `performance.csv`:

- the result of the last call, and of every counted call, must equal the expected result, within a
  relative tolerance of 1e-6;
- a typical call, the median of the counted calls, must not allocate more than the allocation bound. A
  bound of `0` is stricter: no counted call may allocate;
- a typical call must not keep more memory than the retained-memory bound: the bytes it allocates and
  has not freed when it returns. A bound of `0` is stricter: no counted call may keep any;
- the peak private memory of the process must not exceed the peak bound, in megabytes. That peak is
  the one of the process since it started: the bound is checked only for the first benchmark of a
  process, as ctest runs them, and reported as skipped for the next ones.

The bounds apply to a typical call because some calls legitimately allocate more than others: a value
put in a cache, a container that grows now and then. An allocation bound of `0` states that a path does
not allocate after preparation, which is what
[Architecture.md](Architecture.md#preallocate-before-calculation) asks of calculation paths. A
retained-memory bound of `0` states that an operation leaves nothing behind: repeated, it does not grow
the memory of the process.

A computation of complex yields is the exception. FMT keeps in its yields cache the values whose
computation lasted more than 0.05 ms, so the memory such a call keeps depends on the speed of each
computation: a preemption during a counted call can leave a few values in the cache, and the table of
the cache when it was still empty. The retained-memory bound of a benchmark that computes complex
yields is therefore the most the cache can keep during one call, with 10 % added. That worst case is
measured by forcing every computed value into the cache: the allocations that
`FMTComplexYieldHandler::get` makes during the counted call are slowed down beyond 0.05 ms. The
harness has no option for it yet.

A peak bound is the highest peak of two measurements in each mode, with 10 % added: it lets a flow
vary as it does from one run to the next, and fails when a change makes it need much more memory.

An allocation count that depends on the machine takes no bound: the count of a project read, for
instance, grows with the length of the path of the project. It is still measured and compared.

No check applies to the times. Timing thresholds may come once enough reference results exist.

## The allocation monitor

Under MSVC, `operator new` is linked into each module, so replacing it in the executable would not see
the allocations made inside `FMTlib.dll`. Every module ends up calling the heap functions of the C
runtime through its import table: `Performance::AllocationMonitor::install` redirects `malloc`,
`calloc`, `realloc`, `free`, `_aligned_malloc`, `_aligned_realloc` and `_aligned_free` in the import
table of every loaded module, including the solvers and Boost, to functions that count and forward the
call. FMT is not modified, and the measured build is the delivered one.

By default, only the allocations of the thread that runs the benchmark are counted. A benchmark that
hands its work to other threads, such as the replanning, counts those of every thread
(`Benchmark::getThreadScope`).

Not counted:

- allocations made inside the C runtime itself, such as `strdup` or stdio buffers;
- direct calls to `HeapAlloc` or `VirtualAlloc`;
- modules linked to a static C runtime.

Aligned blocks are counted, but left out of the live bytes. Outside Windows the monitor is not
available: a row with an allocation or retained-memory bound is then reported as skipped.

The memory a call keeps is the difference of the live bytes before and after it. A block counts in the
live bytes for the size `_msize` reports, at its allocation and at its release, so the retained bytes of
a call are exact whatever the allocator. On the C runtime heap, that size is the size asked for, one byte
for a request of zero bytes; under mimalloc, it is the size of the class of the block, 16 bytes for 10
asked for. The allocated bytes stay the sizes asked for, and compare from one allocator to another.

## The allocator

The benchmarks run on the C runtime heap, the heap of FMT in production: Python, Excel and the .NET
interface load FMTlib after the C runtime has started, and mimalloc can only replace that heap before.

With MSVC, a build configured with `-DWITH_MIMALLOC=ON` links mimalloc first into every executable, the
benchmarks included, and mimalloc then serves `malloc` in their processes (see
[AGENTS.md, Building](../AGENTS.md#building)). Such a build measures what mimalloc would bring, and is
not for testing: most flows exceed their memory bounds there. To measure the C runtime heap with its
binaries, run with `MIMALLOC_DISABLE_REDIRECT=1`. In PowerShell, set it with
`$env:MIMALLOC_DISABLE_REDIRECT="1"` first.

```bash
MIMALLOC_DISABLE_REDIRECT=1 FMT_BENCHMARK_MODE=full ctest --test-dir build/release -C Release -L performance
```

The `allocator` field of the results then reads `CRT heap (mimalloc 2.1.2 loaded, not redirected)`.

Such a build also has the ctest test `FMTPerformanceTests.Mimalloc`, which checks that the benchmarks do
run on mimalloc: it runs one benchmark and fails when the allocator in its header is not mimalloc, as
happens when mimalloc is no longer the first import of the executable. It has no label, so a measurement
leaves it out, and it fails, as it should, under `MIMALLOC_DISABLE_REDIRECT=1`.

The counts do not depend on the allocator: when mimalloc redirects `malloc`, the calls still go through
the redirected imports, and the benchmarks count the same allocations. Durations, peaks and retained
bytes do depend on it, which is why every result records the allocator of its run. Under mimalloc, the
peak of a process of the benchmarks rises from about 22 MB to 54 MB: the peak bounds, and the
retained-memory bounds that are not 0, hold for the C runtime heap only.

## Results

Each run writes one JSON file: the environment of the run, then one entry per benchmark. The layout is
version 3 of the schema; any change to it changes `schemaVersion` and this section. Version 3 adds
`datasetFingerprint`, `phases` and `maxPeakMemoryMB`; version 2 added `processors`, the
retained-memory fields and `maxRetainedBytesPerCall`. `CompareResults.cmake` still reads versions 1
and 2.

| Environment field | Meaning |
| --- | --- |
| `fmtVersion`, `fmtBuildDate` | Version and build date of `FMTlib`. |
| `features` | Optional components of `FMTlib`, as reported by `Version::FMTVersion::hasFeature`. |
| `allocator` | Allocator that serves `malloc` in the process: `CRT heap` without mimalloc, `mimalloc <version>` when mimalloc redirects the C runtime, in a build configured with `-DWITH_MIMALLOC=ON`, and `CRT heap (mimalloc <version> loaded, not redirected)` when mimalloc is loaded but does not serve `malloc`, as with `MIMALLOC_DISABLE_REDIRECT=1`. |
| `commit`, `dirty` | Commit of the sources when the benchmarks were built, and whether the working tree then differed from it (modified or untracked files). |
| `buildType`, `optimized` | Configuration of the build, and whether it was compiled with `NDEBUG`. |
| `compiler`, `compilerVersion` | Compiler of the benchmarks. |
| `os`, `cpu`, `logicalCores` | Machine. |
| `processors` | What the measuring thread ran on, and whether Windows could throttle the process: `fastest cores, 16 of 32 logical processors, not throttled` on a hybrid processor. |
| `availableMemoryBytes` | Physical memory available at the start of the run. |
| `timestamp` | Start of the run, in UTC. |
| `mode` | `smoke` or `full`. |

| Result field | Meaning |
| --- | --- |
| `benchmark`, `group`, `dataset`, `threads` | What was measured, on which data, with how many threads. |
| `datasetFingerprint` | SHA-256 of the files of the dataset, for a flow or a private benchmark (see [Private benchmarks](#private-benchmarks)); empty otherwise. |
| `samples`, `callsPerSample` | Timed samples; their product is the number of timed calls. |
| `minNs`, `maxNs`, `medianNs`, `meanNs`, `stddevNs` | Duration of one call over the samples, in nanoseconds. |
| `phases` | One entry per phase of a call, in order: `name`, `minNs`, `medianNs`, `maxNs`. Empty for an operation timed as a whole. |
| `allocationCalls` | Counted calls. |
| `allocationsPerCallMin`, `allocationsPerCallMedian`, `allocationsPerCallMax` | Allocations of one counted call. |
| `allocatedBytesPerCallMedian` | Bytes allocated by a typical call. |
| `retainedBytesPerCallMedian`, `retainedBytesPerCallMax` | Bytes a counted call keeps allocated when it returns: typical call and maximum. |
| `allocations`, `deallocations`, `allocatedBytes`, `retainedBytes` | Totals over the counted calls; `retainedBytes` is what all of them kept. |
| `peakLiveHeapBytes` | Highest amount of memory allocated and not yet freed during the counted calls. |
| `processPeakPrivateBytes` | Peak private memory of the process at the end of the benchmark. |
| `result`, `expected`, `maxAllocationsPerCall`, `maxRetainedBytesPerCall`, `maxPeakMemoryMB` | Result of the last counted call, and the expectation it was checked against. |
| `valid`, `skipped`, `skipReason`, `failures` | Outcome of the checks. |

The allocation fields are `null` where the monitor is not available.

## Comparing two runs

```bash
cmake -DBASELINE=<file or folder> -DCANDIDATE=<file or folder> -P Tests/Performance/CompareResults.cmake
```

A folder stands for every `.json` file it holds, such as `build/release/tests/performance` after a
measurement. For each benchmark, the report gives the change of the median duration and of each phase,
the allocations and bytes of a typical call before and after, and the change of the process peak
memory:

```text
Flow.Optimize
  Median duration:  -1.3%  (30.424 ms -> 30.003 ms)
    read:  -6.6%  (4.848 ms -> 4.526 ms)
    build:  +0.2%  (1.954 ms -> 1.959 ms)
    solve:  +0.2%  (21.562 ms -> 21.617 ms)
  Allocations:      47167 -> 47167 per call
  Allocated bytes:  5566717 -> 5566717 per call
  Retained bytes:   -792 -> -792 per call
  Peak memory:      +0.0%  (process peak private bytes)
```

When the fingerprints of a dataset differ, the report warns that the results do not compare.

Durations only compare between two measurements of the same mode, on the same machine and kind of cores,
in the same build type and with the same allocator: the report warns when they differ. Two measurements
of the same commit differ by up to 8 % on the machine where the suite was written. The comparison only
reports; nothing fails on a change.

To show the effect of a change, measure the commit before it, keep the results out of the build folder,
measure the commit with it, and compare. To show the effect of the allocator, measure a build configured
with `-DWITH_MIMALLOC=ON` with and without `MIMALLOC_DISABLE_REDIRECT=1`, in turns, and compare: the
warning on the allocators then names the very thing measured.

## Adding a benchmark

1. **Write the benchmark** in the `*Benchmarks.cpp` file of its group, or in a new one added to
   `Tests/Performance/CMakeLists.txt`. Its name is `<group>.<operation>[.<variant>]`. Everything the
   measured operation needs is built in `prepare`; `run` performs the operation once and returns a
   result that depends on it. An operation of milliseconds or more overrides `getSettings` to return
   `RunSettings::forSlowCalls`, a model flow `RunSettings::forFlows`. A benchmark that reads a model
   calls `Performance::quietFmt()` first in `prepare`. FMT keeps one logger for the whole process, and
   the solver of a model keeps the message handler of the logger it was built with: replacing the logger
   while a model exists leaves that model with a destroyed handler, and copying the model then fails or
   crashes the process. Never replace the FMT logger in a benchmark.
2. **Add it to the suite** in `FMTPerformanceTests.cpp`.
3. **Compute the expected result by hand**, from the files of the model, never from what FMT prints:
   a benchmark that measures a wrong computation must fail.
4. **Add its row** to `performance.csv`:
   `FMTPerformanceTests;<name>;<expected result>;<allocation bound>;<retained-memory bound>;<peak MB>`.
   Leave the bounds empty for a first run.
5. **Measure the bounds**: run the benchmark twice and read the allocations and the retained bytes per
   call. Write each median as a bound when it changes neither between the runs nor with the machine. For
   a flow, run it alone in its process twice in each mode, and write the highest peak with 10 % added.
   A benchmark that computes complex yields bounds its retained memory at the most the yields cache can
   keep (see [What a benchmark does](#what-a-benchmark-does)).
6. **See each check fail** without recompiling: `--expected` with a wrong value, `--max-allocations`,
   `--max-retained-bytes` and `--max-peak-memory` below the measure.
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

### Age yields

`Yield.Age` reads the age yield `VOLUMETOTAL` through `Core::FMTYields::get`, with the requests of the
complex yields: 120, interpolated between 100 at age 5 and 150 at age 10. It allocates nothing.
`Yield.Age.NewRequest` asks through a new request at every call, as happens when a development is met
for the first time: the request first locates the yield data of its development, which costs 7
allocations.

### Masks

On the themes of the root of TWD_land:

| Benchmark | Operation | Expected result |
| --- | --- | --- |
| `Mask.IsSubsetOf` | Tests whether `UNITE1 PEUPLEMENT1 UTR1` belongs to `UC PROD ?` | 1 |
| `Mask.FromString` | Builds the mask `UC PROD ?` from its text, and counts its bits: 2 + 3 + 3 | 8 |

`Mask.IsSubsetOf` allocates nothing.

### Project reads

`Parser.ReadProject` reads the root of TWD_land with a new parser at every call, and returns its initial
area, 1814.76 ha. `Parser.ReadProject.TwoScenarios` reads the root and the `perfyields` scenario in the
same call: 3629.52 ha for the two models. The models are destroyed before the call returns, and a read
must keep no memory. The allocations of a read are measured, but not bounded: their count grows with the
length of the path of the project.

### Model flows

On TWD_land, each flow reads its model at every call:

| Benchmark | Data | Phases | Expected result |
| --- | --- | --- | --- |
| `Flow.Optimize` | `NOT_MASK`, 5 periods | read, build, solve | objective 90738, as `doplanning` checks it |
| `Flow.Optimize.Long` | `NOT_MASK`, 20 periods: a graph twelve times larger | read, build, solve | objective 362952 |
| `Flow.Replay` | schedule of `LP`, 10 periods, without a solver | read, build | `OVOLREC` at period 2: 48008.953705 |
| `Flow.Outputs` | schedule of `LP`, 10 periods, built once by `prepare` | none | sum of the totals of every output of every period: 3079696.8362052 |
| `Flow.Simulate` | `DECISION`, non-spatial simulation | read, simulate | `UNIT_REC` at period 5: 60, as `FMTNsstest` checks it |
| `Flow.Replanning` | `Globalreplanning`, `Globalfire` and `Localreplanning`: 2 replicates of 5 periods, on one thread | read, setup, replanning, result | 20 rows written: 2 replicates × 5 periods × 2 outputs |

The objectives are the same with MOSEK and CLP, and the results of the replay, the outputs and the
simulation do not depend on the solver. The replanned values, however, depend on which optimal solution
the solver returns: for the same replicates, the local model harvests 447 126 m³ under MOSEK and 465 421
m³ under CLP. The replanning therefore checks the number of rows it writes. Each run of a replanning
keeps about 4.7 MB, whatever the number of its replicates and periods: its retained-memory bound is 5 MB,
so that the bound fails if the memory kept starts to grow with the replicates. `Flow.Outputs` is the only
flow that computes complex yields: its retained-memory bound, 22 000 bytes, covers the 19 987 bytes that
the yields cache can keep during one call. Every flow bounds the peak memory of its process.

### Private benchmarks

The rows of `Tests/Performance/performance-private.csv`, a local file that git ignores, measure flows on
production models. They have the columns of `performance.csv`, followed by `ARGUMENTS`. The name of a
private benchmark is its kind followed by a variant, `<kind>.<variant>`, and its arguments, separated by
`|`, give its model:

| Kind | Arguments |
| --- | --- |
| `Flow.Optimize` | `<primary file>\|<scenario>\|<length>` |
| `Flow.Replay` | `<primary file>\|<scenario>\|<length>\|<output>\|<period>` |
| `Flow.Outputs` | `<primary file>\|<scenario>\|<length>\|<outputs, a count or all>` |
| `Flow.Simulate` | `<primary file>\|<scenario>\|<length>\|<output>\|<period>` |
| `Flow.Replanning` | `<primary file>\|<global scenario>\|<stochastic scenario>\|<local scenario>\|<global length>\|<replanned periods>\|<replicates>\|<outputs joined by +>` |
| `Yield.Model` | `<primary file>\|<scenario>\|<yield>\|<developments>` |

The model must be on `T:\`, like those of the private tests: the name of its ctest test holds the path
of the model, which keeps the test out of the base suite, `-E "T:/"`. CMake warns and does not register a
row whose model is elsewhere. Each private benchmark records the SHA-256 of the files of its model, the
files beside its primary file and those of the scenarios it reads: these models are not versioned, and
two measurements compare only on the same fingerprint. The file, the names of the models and the results
stay on the machine.

On a production model, a flow reads the errors that `doplanning` turns into warnings as warnings, so
that the model reads and plans as it does in production (`Performance::quietFmt`). A private flow that
computes complex yields bounds its retained memory at the most the yields cache can keep during one
call: on a production model, several megabytes.
