@echo off
rem Runs the private system tests, the rows of the local BFECtests.csv on the models of T:\, then prints
rem the summary of the pass: RunTests.bat system-private. The campaign takes about 168 minutes.
rem Usage: tools\tests\RunPrivateTests.bat [ctest options...]
rem   Example: tools\tests\RunPrivateTests.bat -R doplanning
rem The private benchmarks, labelled bfec-perf, run apart: see Documentation/PerformanceTesting.md.
rem The window waits for a key, then the script returns the exit code of ctest.
setlocal

if not exist "T:\" (
    echo T:\ is not reachable: the private tests read their models there.
    pause
    endlocal & exit /b 1
)

call "%~dp0RunTests.bat" system-private %*
endlocal & exit /b %ERRORLEVEL%
