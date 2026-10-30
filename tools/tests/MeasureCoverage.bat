@echo off
setlocal EnableExtensions
REM ============================================================================
REM  MeasureCoverage.bat
REM  Measures the line and function coverage of FMTlib by the public C++ tests,
REM  level by level, on the coverage build (build\coverage), and keeps the
REM  reports out of the repository (Measure-Coverage.ps1, whose help gives the
REM  configuration of the build). The arguments go to the script:
REM    MeasureCoverage.bat
REM    MeasureCoverage.bat -Levels unit,integration
REM    MeasureCoverage.bat -Output D:\coverage -BuildDir build\coverage
REM  Without argument, as from the Explorer: the unit, integration and system
REM  levels, then a pause.
REM ============================================================================

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Measure-Coverage.ps1" %*
set "CODE=%ERRORLEVEL%"
if "%~1"=="" pause
endlocal & exit /b %CODE%
