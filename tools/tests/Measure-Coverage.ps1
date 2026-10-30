<#
.SYNOPSIS
Measures the line and function coverage of FMTlib by the public C++ tests, level by level, with the
code coverage tool of Visual Studio, and keeps the reports out of the repository.

.DESCRIPTION
Measures a build made for coverage, build\coverage by default: Release, without optimization, with
full debug information and /PROFILE at link time, which the static instrumentation needs:

    cmake -S . -B build/coverage <the options of tools\build\CMakeFMTVS2022vcpkg_GL.bat>
        "-DCMAKE_CXX_FLAGS_RELEASE=/Od /Ob0 /Zi /DNDEBUG"
        "-DCMAKE_SHARED_LINKER_FLAGS_RELEASE=/INCREMENTAL:NO /DEBUG:FULL /PROFILE"
        "-DCMAKE_EXE_LINKER_FLAGS_RELEASE=/INCREMENTAL:NO /DEBUG:FULL /PROFILE"
        -DINSTALL_TEST_SUITE=none
    cmake --build build/coverage --config Release
    del build\coverage\bin\Release\setup.py
    cmake --install build/coverage --config Release

Do not set WITHOUT_PYTHONLIB: with MSVC, FMTlib then has no output directory and the wrappers do not
link. Delete setup.py before the install, otherwise the install builds the Python wheel of this build
and installs it with pip in the Python that the configuration found.

The script instruments FMTlib.dll, FMTWrapperCore.dll and the executables of the tests of each level
(Microsoft.CodeCoverage.Console instrument), runs each level under the tool with ctest (collect), never
the private tests, then gives every binary back as it was (uninstrument), even after a failure. Only
the sources of Source\ and Include\ count. A line is counted once, covered when any module ran it,
and a function once, by file, class, name and first line.

The reports, Cobertura and .coverage, the ctest logs and summary.txt are written in
<Output>\<date>_<commit>[-modified]_coverage. Python, R, Excel, the private tests, the benchmarks and the
disabled cases are not measured, and code that no module contains, such as a template never
instantiated, is not counted.

.EXAMPLE
Measure-Coverage.ps1
The unit, integration and system levels of build\coverage.

.EXAMPLE
Measure-Coverage.ps1 -Levels unit,integration
The unit and integration levels only.
#>
param(
    # Levels to measure, among unit, integration and system (the C++ system tests), separated by
    # commas: "unit,integration" (powershell -File passes a list as one string).
    [string[]] $Levels = @("unit", "integration", "system"),

    # Number of tests ctest runs at once.
    [ValidateRange(1, 64)]
    [int] $Jobs = 8,

    # Folder that keeps the reports. Default: coverage-reports beside the repository.
    [string] $Output = "",

    # Build to measure. Default: build\coverage in the repository.
    [string] $BuildDir = "",

    # Microsoft.CodeCoverage.Console.exe. Default: the one of the latest Visual Studio that has it.
    [string] $Tool = "",

    # Repository. Default: the one that holds this script.
    [string] $Root = ""
)

$ErrorActionPreference = "Stop"

###############################################################################
# Helper functions
###############################################################################

# Path of ctest.exe: the one in the PATH, else the one of Visual Studio.
function Find-CTest
{
    $command = Get-Command "ctest.exe" -ErrorAction SilentlyContinue
    if ($command)
    {
        return $command.Source
    }
    foreach ($installation in Get-VisualStudioInstallations)
    {
        $candidate = Join-Path $installation "Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe"
        if (Test-Path $candidate)
        {
            return $candidate
        }
    }
    throw "ctest.exe not found: install CMake, or Visual Studio with its CMake tools."
}

# Installation folders of Visual Studio, the latest first.
function Get-VisualStudioInstallations
{
    $vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere))
    {
        return @()
    }
    return @(& $vswhere -products * -prerelease -sort -property installationPath)
}

# Path of Microsoft.CodeCoverage.Console.exe, part of Visual Studio Enterprise and Professional.
function Find-CoverageTool
{
    foreach ($installation in Get-VisualStudioInstallations)
    {
        $candidate = Join-Path $installation `
            "Common7\IDE\Extensions\Microsoft\CodeCoverage.Console\Microsoft.CodeCoverage.Console.exe"
        if (Test-Path $candidate)
        {
            return $candidate
        }
    }
    throw "Microsoft.CodeCoverage.Console.exe not found: it comes with Visual Studio Enterprise or Professional, or give -Tool."
}

# Runs a native command, shows and logs everything it writes, and returns its exit code.
function Invoke-Logged
{
    param(
        [Parameter(Mandatory)] [string] $Program,
        [Parameter(Mandatory)] [string[]] $Arguments,
        [Parameter(Mandatory)] [string] $Log
    )

    $previous = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    try
    {
        # Windows PowerShell wraps each line of standard error in an ErrorRecord, whose text is
        # its message: an empty line would otherwise read as the name of its exception.
        & $Program @Arguments 2>&1 | ForEach-Object {
            if ($_ -is [System.Management.Automation.ErrorRecord]) { $_.Exception.Message } else { "$_" }
        } | Tee-Object -FilePath $Log | Out-Host
        return $LASTEXITCODE
    }
    finally
    {
        $ErrorActionPreference = $previous
    }
}

# ctest arguments that select the tests of a level, never the private ones.
function Get-LevelSelection
{
    param([Parameter(Mandatory)] [string] $Level)

    $selection = @("-E", "T:/", "-L", "^$Level$")
    if ($Level -eq "system")
    {
        $selection += @("-LE", "^(python|r)$")
    }
    return $selection
}

# Lines and functions of FMTlib in a Cobertura report: hits by "file|line", and covered or not by
# "file|class|method|first line". A header compiled in several modules counts once.
function Read-Cobertura
{
    param([Parameter(Mandatory)] [string] $Path)

    $lines = @{}
    $methods = @{}
    $namespaces = @{}
    $report = New-Object System.Xml.XmlDocument
    $report.Load($Path)
    foreach ($class in $report.SelectNodes("//class"))
    {
        $file = $class.GetAttribute("filename").ToLowerInvariant()
        if (-not ($file.StartsWith($sourcePrefix) -or $file.StartsWith($includePrefix)))
        {
            continue
        }
        $className = $class.GetAttribute("name")
        $namespace = if ($className.Contains(".")) { $className.Substring(0, $className.IndexOf(".")) } else { "(global)" }
        foreach ($method in $class.SelectNodes("methods/method"))
        {
            $first = [int]::MaxValue
            $hit = $false
            foreach ($line in $method.SelectNodes("lines/line"))
            {
                $number = [int]$line.GetAttribute("number")
                $hits = [long]$line.GetAttribute("hits")
                $key = "$file|$number"
                if (-not $lines.ContainsKey($key))
                {
                    $lines[$key] = 0
                    $namespaces[$key] = $namespace
                }
                if ($hits -gt $lines[$key])
                {
                    $lines[$key] = $hits
                }
                if ($number -lt $first)
                {
                    $first = $number
                }
                $hit = $hit -or ($hits -gt 0)
            }
            if ($first -eq [int]::MaxValue)
            {
                continue
            }
            $key = "$file|$className|$($method.GetAttribute('name'))|$first"
            $methods[$key] = ($methods.ContainsKey($key) -and $methods[$key]) -or $hit
            $namespaces["m|$key"] = $namespace
        }
    }
    return @{ Lines = $lines; Methods = $methods; Namespaces = $namespaces }
}

# "covered / measured (percent)".
function Format-Ratio
{
    param([int] $Covered, [int] $Total)

    if ($Total -eq 0)
    {
        return "0 / 0"
    }
    return "{0} / {1} ({2:N1} %)" -f $Covered, $Total, (100.0 * $Covered / $Total)
}

###############################################################################
# Paths, build and tools
###############################################################################

$Levels = @($Levels | ForEach-Object { $_ -split "[,\s]+" } | Where-Object { $_ })
foreach ($level in $Levels)
{
    if ($level -notin @("unit", "integration", "system"))
    {
        throw "Unknown level $level`: the levels are unit, integration and system."
    }
}
if ($Levels.Count -eq 0)
{
    throw "No level to measure."
}

if (-not $Root)
{
    $Root = Join-Path $PSScriptRoot "..\.."
}
$Root = (Resolve-Path $Root).ProviderPath
if (-not $BuildDir)
{
    $BuildDir = Join-Path $Root "build\coverage"
}
if (-not (Test-Path $BuildDir))
{
    throw "No build in $BuildDir`: configure, build and install the coverage build first (Get-Help $PSCommandPath)."
}
$BuildDir = (Resolve-Path $BuildDir).ProviderPath
$binDir = Join-Path $BuildDir "bin\Release"
foreach ($file in @("FMTlib.dll", "FMTlib.pdb"))
{
    if (-not (Test-Path (Join-Path $binDir $file)))
    {
        throw "No $file in $binDir`: build and install the coverage build first (Get-Help $PSCommandPath)."
    }
}
if (-not $Tool)
{
    $Tool = Find-CoverageTool
}
$ctest = Find-CTest
$sourcePrefix = (Join-Path $Root "Source\").ToLowerInvariant()
$includePrefix = (Join-Path $Root "Include\").ToLowerInvariant()

$commit = "unknown"
$dirty = $false
if (Get-Command "git.exe" -ErrorAction SilentlyContinue)
{
    $head = & git -C $Root rev-parse --short=8 HEAD 2>$null
    if ($LASTEXITCODE -eq 0 -and $head)
    {
        $commit = "$head".Trim()
        $dirty = [bool](& git -C $Root status --porcelain --untracked-files=no 2>$null)
    }
}
if (-not $Output)
{
    $Output = Join-Path (Split-Path $Root -Parent) "coverage-reports"
}
$Output = [System.IO.Path]::GetFullPath($Output)
$prefix = "{0}_{1}{2}_coverage" -f (Get-Date -Format "yyyy-MM-dd"), $commit, $(if ($dirty) { "-modified" } else { "" })
$number = 1
do
{
    $runFolder = Join-Path $Output $(if ($number -eq 1) { $prefix } else { "{0}_{1}" -f $prefix, $number })
    $number++
} while (Test-Path $runFolder)
New-Item -ItemType Directory -Force $runFolder | Out-Null

Write-Host ""
Write-Host "Build:   $BuildDir" -ForegroundColor Cyan
Write-Host ("Commit:  {0}{1}" -f $commit, $(if ($dirty) { " (modified)" } else { "" }))
Write-Host "Levels:  $($Levels -join ', ')"
Write-Host "Tool:    $Tool"
Write-Host "Reports: $runFolder"

###############################################################################
# Modules to instrument and settings of the tool
###############################################################################

$modules = New-Object System.Collections.Generic.SortedSet[string]
[void]$modules.Add("FMTlib.dll")
if (Test-Path (Join-Path $binDir "FMTWrapperCore.dll"))
{
    [void]$modules.Add("FMTWrapperCore.dll")
}
foreach ($level in $Levels)
{
    $previous = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    $json = (& $ctest --test-dir $BuildDir -C Release --show-only=json-v1 @(Get-LevelSelection $level) 2>$null) -join "`n"
    $ErrorActionPreference = $previous
    if ($LASTEXITCODE -ne 0)
    {
        throw "ctest could not list the $level tests of $BuildDir."
    }
    foreach ($test in (ConvertFrom-Json $json).tests)
    {
        if (-not $test.command)
        {
            continue
        }
        $program = [System.IO.Path]::GetFullPath($test.command[0])
        if ((Split-Path $program -Parent) -eq $binDir.TrimEnd("\"))
        {
            [void]$modules.Add((Split-Path $program -Leaf))
        }
    }
}

$config = Join-Path $runFolder "coverage.config"
$settings = @"
<?xml version="1.0" encoding="utf-8"?>
<Configuration>
  <CodeCoverage>
    <ModulePaths>
      <Include>
        <ModulePath>$([regex]::Escape($binDir + "\"))[^\\]*\.(dll|exe)$</ModulePath>
      </Include>
    </ModulePaths>
    <Sources>
      <Include>
        <Source>$([regex]::Escape($sourcePrefix)).*</Source>
        <Source>$([regex]::Escape($includePrefix)).*</Source>
      </Include>
    </Sources>
    <CollectFromChildProcesses>True</CollectFromChildProcesses>
    <EnableStaticNativeInstrumentation>True</EnableStaticNativeInstrumentation>
    <EnableDynamicNativeInstrumentation>False</EnableDynamicNativeInstrumentation>
  </CodeCoverage>
</Configuration>
"@
[System.IO.File]::WriteAllText($config, $settings, (New-Object System.Text.UTF8Encoding($false)))

###############################################################################
# Instrumentation, runs and reports
###############################################################################

$failures = 0
$sessionId = [guid]::NewGuid().ToString()
$previousTelemetry = $env:DOTNET_COVERAGE_TELEMETRY_OPTOUT
$previousLogo = $env:DOTNET_COVERAGE_NOLOGO
$env:DOTNET_COVERAGE_TELEMETRY_OPTOUT = "1"
$env:DOTNET_COVERAGE_NOLOGO = "1"
try
{
    # A run stopped before its end leaves instrumented binaries: give them back first.
    foreach ($original in @(Get-ChildItem $binDir -Filter "*.orig"))
    {
        $module = Join-Path $binDir $original.BaseName
        Write-Host "Restoring $module, left instrumented by a former run." -ForegroundColor Yellow
        & $Tool uninstrument $module | Out-Null
    }

    Write-Host ""
    Write-Host "Instrumenting $($modules.Count) modules" -ForegroundColor Cyan
    foreach ($module in $modules)
    {
        $code = Invoke-Logged $Tool @("instrument", (Join-Path $binDir $module), "-s", $config,
            "--session-id", $sessionId) (Join-Path $runFolder "instrument_$module.log")
        if ($code -ne 0)
        {
            throw "Instrumenting $module failed (code $code): is the build linked with /PROFILE?"
        }
    }

    $levelReports = @{}
    foreach ($level in $Levels)
    {
        Write-Host ""
        Write-Host "Level $level" -ForegroundColor Cyan
        $coverage = Join-Path $runFolder "$level.coverage"
        $arguments = @("collect", "--session-id", $sessionId, "-s", $config, "-f", "coverage", "-o", $coverage,
            $ctest, "--test-dir", $BuildDir, "-C", "Release", "-j", "$Jobs", "--output-on-failure") +
            @(Get-LevelSelection $level)
        $code = Invoke-Logged $Tool $arguments (Join-Path $runFolder "ctest_$level.log")
        if ($code -ne 0)
        {
            $failures++
            Write-Host "Level $level failed (code $code): see ctest_$level.log" -ForegroundColor Red
        }
        if (-not (Test-Path $coverage))
        {
            $failures++
            Write-Host "Level $level gave no coverage." -ForegroundColor Red
            continue
        }
        $cobertura = Join-Path $runFolder "$level.cobertura.xml"
        $code = Invoke-Logged $Tool @("merge", $coverage, "-f", "cobertura", "-o", $cobertura) `
            (Join-Path $runFolder "merge_$level.log")
        if ($code -ne 0)
        {
            throw "Converting the $level coverage failed (code $code)."
        }
        $levelReports[$level] = $cobertura
    }

    $coverages = @($Levels | Where-Object { $levelReports.ContainsKey($_) } |
        ForEach-Object { Join-Path $runFolder "$_.coverage" })
    if ($coverages.Count -gt 0)
    {
        $total = Join-Path $runFolder "total.cobertura.xml"
        $code = Invoke-Logged $Tool (@("merge") + $coverages + @("-f", "cobertura", "-o", $total)) `
            (Join-Path $runFolder "merge_total.log")
        if ($code -ne 0)
        {
            throw "Merging the levels failed (code $code)."
        }
    }
}
finally
{
    foreach ($original in @(Get-ChildItem $binDir -Filter "*.orig"))
    {
        & $Tool uninstrument (Join-Path $binDir $original.BaseName) | Out-Null
    }
    $left = @(Get-ChildItem $binDir -Filter "*.orig")
    if ($left.Count -gt 0)
    {
        Write-Host "Still instrumented: $($left.BaseName -join ', ')" -ForegroundColor Red
    }
    Get-ChildItem $binDir -Filter "*.instr.pdb" | Remove-Item -ErrorAction SilentlyContinue
    if ($null -eq $previousTelemetry) { Remove-Item Env:DOTNET_COVERAGE_TELEMETRY_OPTOUT -ErrorAction SilentlyContinue }
    else { $env:DOTNET_COVERAGE_TELEMETRY_OPTOUT = $previousTelemetry }
    if ($null -eq $previousLogo) { Remove-Item Env:DOTNET_COVERAGE_NOLOGO -ErrorAction SilentlyContinue }
    else { $env:DOTNET_COVERAGE_NOLOGO = $previousLogo }
}

###############################################################################
# Summary
###############################################################################

$summary = New-Object System.Collections.Generic.List[string]
$summary.Add("Coverage of FMTlib (Source and Include), $commit$(if ($dirty) { ' (modified)' }), $(Get-Date -Format 'yyyy-MM-dd HH:mm')")
$summary.Add("")
$summary.Add(("{0,-12} {1,-28} {2}" -f "Level", "Lines", "Functions"))
$names = @($Levels | Where-Object { $levelReports.ContainsKey($_) })
if (Test-Path (Join-Path $runFolder "total.cobertura.xml"))
{
    $names += "total"
}
$totalData = $null
foreach ($name in $names)
{
    $data = Read-Cobertura (Join-Path $runFolder "$name.cobertura.xml")
    $coveredLines = @($data.Lines.Values | Where-Object { $_ -gt 0 }).Count
    $coveredMethods = @($data.Methods.Values | Where-Object { $_ }).Count
    $summary.Add(("{0,-12} {1,-28} {2}" -f $name, (Format-Ratio $coveredLines $data.Lines.Count),
        (Format-Ratio $coveredMethods $data.Methods.Count)))
    if ($name -eq "total")
    {
        $totalData = $data
    }
}
if ($totalData)
{
    $byNamespace = @{}
    foreach ($key in $totalData.Lines.Keys)
    {
        $namespace = $totalData.Namespaces[$key]
        if (-not $byNamespace.ContainsKey($namespace)) { $byNamespace[$namespace] = @(0, 0, 0, 0) }
        $byNamespace[$namespace][1]++
        if ($totalData.Lines[$key] -gt 0) { $byNamespace[$namespace][0]++ }
    }
    foreach ($key in $totalData.Methods.Keys)
    {
        $namespace = $totalData.Namespaces["m|$key"]
        if (-not $byNamespace.ContainsKey($namespace)) { $byNamespace[$namespace] = @(0, 0, 0, 0) }
        $byNamespace[$namespace][3]++
        if ($totalData.Methods[$key]) { $byNamespace[$namespace][2]++ }
    }
    $summary.Add("")
    $summary.Add(("{0,-38} {1,-28} {2}" -f "Namespace (total)", "Lines", "Functions"))
    foreach ($namespace in ($byNamespace.Keys | Sort-Object))
    {
        $counts = $byNamespace[$namespace]
        $summary.Add(("{0,-38} {1,-28} {2}" -f $namespace, (Format-Ratio $counts[0] $counts[1]),
            (Format-Ratio $counts[2] $counts[3])))
    }
}
$summaryFile = Join-Path $runFolder "summary.txt"
[System.IO.File]::WriteAllLines($summaryFile, $summary, (New-Object System.Text.UTF8Encoding($false)))

Write-Host ""
$summary | Out-Host
Write-Host ""
Write-Host "Reports: $runFolder" -ForegroundColor Cyan
if ($failures -gt 0)
{
    Write-Host "$failures level(s) failed." -ForegroundColor Red
    exit 1
}
exit 0
