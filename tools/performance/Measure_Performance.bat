@echo off
setlocal EnableExtensions
REM ============================================================================
REM  Measure_Performance.bat
REM  Measures the performance benchmarks in full mode, keeps the results out of
REM  the build and compares them with reference runs (Measure-Performance.ps1,
REM  Documentation\PerformanceTesting.md). The arguments go to the script:
REM    Measure_Performance.bat
REM    Measure_Performance.bat -Baseline ..\perf-references\<date>_<commit>_performance_*
REM    Measure_Performance.bat -Label bfec-perf -Runs 2
REM  Without argument, as from the Explorer: three runs of the public
REM  benchmarks, without comparison, then a pause.
REM ============================================================================

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Measure-Performance.ps1" %*
set "CODE=%ERRORLEVEL%"
if "%~1"=="" pause
endlocal & exit /b %CODE%
