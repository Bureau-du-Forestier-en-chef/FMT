@echo off
rem Runs a test preset of CMakePresets.json, then prints the summary of the pass by level.
rem Usage: RunTests.bat [preset] [ctest options...]
rem   preset: unit, integration, system, system-private, performance or all-public (the default).
rem   Example: RunTests.bat system -R Scenario
rem The window waits for a key, then the script returns the exit code of ctest.
setlocal
cd /d "%~dp0"

set "PRESET=all-public"
set "OPTIONS="
set "FIRST=%~1"
if not defined FIRST goto :findctest
if "%FIRST:~0,1%"=="-" goto :nextoption
set "PRESET=%~1"
shift
:nextoption
if "%~1"=="" goto :findctest
set OPTIONS=%OPTIONS% %1
shift
goto :nextoption

:findctest
rem ctest from the PATH, otherwise the one of the CMake of Visual Studio 2022.
set "CTEST="
for /f "delims=" %%I in ('where ctest 2^>nul') do if not defined CTEST set "CTEST=%%I"
for %%E in (Professional Enterprise Community BuildTools) do call :tryvisualstudio %%E
if not defined CTEST (
    echo ctest was not found: add the bin folder of CMake to the PATH, or install the CMake
    echo component of Visual Studio 2022.
    set "RC=1"
    goto :end
)
for %%I in ("%CTEST%") do set "CMAKE=%%~dpIcmake.exe"
if not exist "%CMAKE%" set "CMAKE=cmake"

set "LIST=%TEMP%\FMT-RunTests-%PRESET%.json"
set "RESULTS=%TEMP%\FMT-RunTests-%PRESET%.xml"
set "LIST=%LIST:\=/%"
set "RESULTS=%RESULTS:\=/%"
del /q "%LIST%" "%RESULTS%" 2>nul

rem Listed before the pass: every call of ctest rewrites LastTest.log.
"%CTEST%" --preset %PRESET% %OPTIONS% --show-only=json-v1 > "%LIST%"
if errorlevel 1 (
    echo ctest could not list the tests of the preset %PRESET%.
    set "RC=1"
    goto :end
)

"%CTEST%" --preset %PRESET% %OPTIONS% --output-junit "%RESULTS%"
set "RC=%ERRORLEVEL%"

"%CMAKE%" "-DTEST_RESULTS=%RESULTS%" "-DTEST_LIST=%LIST%" -P "%~dp0Tests\Support\TestSummary.cmake"

:end
pause
endlocal & exit /b %RC%

:tryvisualstudio
if defined CTEST goto :eof
set "CANDIDATE=%ProgramFiles%\Microsoft Visual Studio\2022\%1\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe"
if exist "%CANDIDATE%" set "CTEST=%CANDIDATE%"
goto :eof
