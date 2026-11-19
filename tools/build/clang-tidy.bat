@echo off
setlocal

set BUILD_DIR=build\clang

if not exist "%BUILD_DIR%\compile_commands.json" (
    echo ERROR: %BUILD_DIR%\compile_commands.json not found.
    echo Run configure first.
    exit /b 1
)

echo.
echo ==========================================
echo Running clang-tidy on all FMT sources
echo ==========================================
echo.

for /R Source %%f in (*.cpp *.cxx *.cc) do (
    echo.
    echo ===== %%f =====
    clang-tidy "%%f" -p "%BUILD_DIR%"
)

for /R Include %%f in (*.h *.hpp *.hh) do (
    echo.
    echo ===== %%f =====
    clang-tidy "%%f" -p "%BUILD_DIR%"
)

echo.
echo Done.