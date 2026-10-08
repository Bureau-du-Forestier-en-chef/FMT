<#
.SYNOPSIS
Measures the performance benchmarks of a Release build in full mode, keeps the results out of the
build, and compares them with reference runs.

.DESCRIPTION
Runs the benchmarks of a ctest label as a measurement does (Documentation/PerformanceTesting.md):
without -j, one benchmark per process, with FMT_BENCHMARK_MODE=full, Runs times. The results of each
run are kept in a folder of their own, <Output>\<date>_<commit>[-modified]_<label>_<n>, named after
the commit the build was made from. With -Baseline, the runs are then compared with the reference
runs by Tests/Performance/CompareResults.cmake, and the report is written beside them.

.EXAMPLE
Measure-Performance.ps1
Three runs of the public benchmarks, without comparison.

.EXAMPLE
Measure-Performance.ps1 -Baseline ..\perf-references\<date>_<commit>_performance_*
Three runs, compared with every reference run the pattern names.

.EXAMPLE
Measure-Performance.ps1 -Label bfec-perf -Runs 2
Two runs of the private benchmarks.
#>
param(
    # Number of runs.
    [ValidateRange(1, 100)]
    [int] $Runs = 3,

    # ctest label of the benchmarks: performance for the public ones, bfec-perf for the private ones.
    [string] $Label = "performance",

    # ctest regular expression that narrows the benchmarks, such as "ComplexYield".
    [string] $Regex = "",

    # Reference runs to compare with: folders or .json files, wildcards allowed.
    [string] $Baseline = "",

    # Folder that keeps the runs. Default: perf-references beside the repository.
    [string] $Output = "",

    # Build to measure. Default: build\release in the repository.
    [string] $BuildDir = "",

    # Repository. Default: the one that holds this script.
    [string] $Root = ""
)

$ErrorActionPreference = "Stop"

###############################################################################
# Helper functions
###############################################################################

# Path of ctest.exe or cmake.exe: the one in the PATH, else the one of Visual Studio.
function Find-CMakeTool
{
    param([Parameter(Mandatory)] [string] $Name)

    $command = Get-Command "$Name.exe" -ErrorAction SilentlyContinue
    if ($command)
    {
        return $command.Source
    }
    $vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vswhere)
    {
        foreach ($installation in @(& $vswhere -products * -property installationPath))
        {
            $candidate = Join-Path $installation "Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\$Name.exe"
            if (Test-Path $candidate)
            {
                return $candidate
            }
        }
    }
    throw "$Name.exe not found: install CMake, or Visual Studio with its CMake tools."
}

# Runs a native command and returns everything it wrote, standard error included, as text.
function Get-NativeOutput
{
    param(
        [Parameter(Mandatory)] [string] $Program,
        [Parameter(Mandatory)] [string[]] $Arguments
    )

    $previous = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    try
    {
        # Windows PowerShell wraps each line of standard error in an ErrorRecord, whose text is
        # its message: an empty line would otherwise read as the name of its exception.
        return @(& $Program @Arguments 2>&1 | ForEach-Object {
            if ($_ -is [System.Management.Automation.ErrorRecord]) { $_.Exception.Message } else { "$_" }
        })
    }
    finally
    {
        $ErrorActionPreference = $previous
    }
}

###############################################################################
# Paths and build
###############################################################################

if (-not $Root)
{
    $Root = Join-Path $PSScriptRoot "..\.."
}
$Root = (Resolve-Path $Root).ProviderPath
if (-not $BuildDir)
{
    $BuildDir = Join-Path $Root "build\release"
}
if (-not (Test-Path $BuildDir))
{
    throw "No build in $BuildDir`: build the Release configuration first, or give -BuildDir."
}
$BuildDir = (Resolve-Path $BuildDir).ProviderPath
if (-not $Output)
{
    $Output = Join-Path (Split-Path $Root -Parent) "perf-references"
}
$Output = [System.IO.Path]::GetFullPath($Output)

$header = Join-Path $BuildDir "generated\FMTBenchmarkHarness\BenchmarkCommit.h"
if (-not (Test-Path $header))
{
    throw "No benchmarks built in $BuildDir ($header is missing): build the Release configuration first."
}
$commit = "unknown"
$match = Select-String -Path $header -Pattern 'FMT_BENCHMARK_COMMIT "([0-9a-fA-F]+)"'
if ($match)
{
    $commit = $match.Matches[0].Groups[1].Value
}
$dirty = [bool](Select-String -Path $header -Pattern 'FMT_BENCHMARK_DIRTY 1' -Quiet)

$ctest = Find-CMakeTool "ctest"
$cmake = Find-CMakeTool "cmake"
$compareScript = Join-Path $Root "Tests\Performance\CompareResults.cmake"
$resultsFolder = Join-Path $BuildDir "tests\performance"

$short = if ($commit.Length -gt 8) { $commit.Substring(0, 8) } else { $commit }
$modified = if ($dirty) { "-modified" } else { "" }
$prefix = "{0}_{1}{2}_{3}" -f (Get-Date -Format "yyyy-MM-dd"), $short, $modified, $Label

Write-Host ""
Write-Host "Build:   $BuildDir" -ForegroundColor Cyan
Write-Host ("Commit:  {0}{1}" -f $commit, $(if ($dirty) { " (modified)" } else { "" }))
Write-Host "Label:   $Label$(if ($Regex) { ", benchmarks matching $Regex" })"
Write-Host "Runs:    $Runs, kept in $Output"
if ($dirty)
{
    Write-Host "The build was made from modified sources: its results do not make a reference." -ForegroundColor Yellow
}

###############################################################################
# Runs
###############################################################################

New-Item -ItemType Directory -Force $Output | Out-Null
$runFolders = @()
$failedRuns = 0
$previousMode = $env:FMT_BENCHMARK_MODE
try
{
    $env:FMT_BENCHMARK_MODE = "full"
    for ($run = 1; $run -le $Runs; $run++)
    {
        $number = 1
        do
        {
            $folder = Join-Path $Output ("{0}_{1}" -f $prefix, $number)
            $number++
        } while (Test-Path $folder)
        New-Item -ItemType Directory $folder | Out-Null

        Write-Host ""
        Write-Host "Run $run of $Runs -> $folder" -ForegroundColor Cyan
        if (Test-Path $resultsFolder)
        {
            Get-ChildItem $resultsFolder -Filter "*.json" | Remove-Item
        }
        $arguments = @("--test-dir", $BuildDir, "-C", "Release", "-L", $Label, "--output-on-failure",
            "--output-log", (Join-Path $folder "ctest.log"))
        if ($Regex)
        {
            $arguments += @("-R", $Regex)
        }
        & $ctest @arguments
        if ($LASTEXITCODE -ne 0)
        {
            $failedRuns++
            Write-Host "Run $run failed (ctest returned $LASTEXITCODE): see $folder\ctest.log" -ForegroundColor Red
        }
        $results = @(Get-ChildItem $resultsFolder -Filter "*.json" -ErrorAction SilentlyContinue)
        if ($results.Count -eq 0)
        {
            Write-Host "Run $run wrote no results." -ForegroundColor Red
            $failedRuns++
            continue
        }
        $results | Copy-Item -Destination $folder
        $runFolders += $folder
    }
}
finally
{
    if ($null -eq $previousMode)
    {
        Remove-Item Env:FMT_BENCHMARK_MODE -ErrorAction SilentlyContinue
    }
    else
    {
        $env:FMT_BENCHMARK_MODE = $previousMode
    }
}

###############################################################################
# Comparison
###############################################################################

$reportFile = ""
if ($Baseline -and $runFolders.Count -gt 0)
{
    $baselineRuns = @(Resolve-Path $Baseline | ForEach-Object { $_.ProviderPath })
    if ($baselineRuns.Count -eq 0)
    {
        throw "No reference run matches $Baseline."
    }
    $report = Get-NativeOutput $cmake @(
        "-DBASELINE=$($baselineRuns -join ';')",
        "-DCANDIDATE=$($runFolders -join ';')",
        "-P", $compareScript)
    $reportFile = Join-Path $Output ("{0}_comparison.txt" -f $prefix)
    $number = 2
    while (Test-Path $reportFile)
    {
        $reportFile = Join-Path $Output ("{0}_comparison_{1}.txt" -f $prefix, $number)
        $number++
    }
    $report | Set-Content -Path $reportFile -Encoding UTF8
    Write-Host ""
    Write-Host "Comparison with $($baselineRuns.Count) reference run(s)" -ForegroundColor Cyan
    $report | Out-Host
}

###############################################################################
# Summary
###############################################################################

Write-Host ""
Write-Host "Results:" -ForegroundColor Cyan
foreach ($folder in $runFolders)
{
    Write-Host "  $folder"
}
if ($reportFile)
{
    Write-Host "Comparison: $reportFile" -ForegroundColor Cyan
}
if ($failedRuns -gt 0)
{
    Write-Host "$failedRuns run(s) failed." -ForegroundColor Red
    exit 1
}
exit 0
