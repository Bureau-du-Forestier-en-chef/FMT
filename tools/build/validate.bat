@echo off

call clang-tidy-fix.cmd

if errorlevel 1 exit /b %errorlevel%

cmake --build build\clang

if errorlevel 1 exit /b %errorlevel%

echo.
echo Validation successful.

