@echo off
setlocal

set BUILD_DIR=build\clang

if not exist "%BUILD_DIR%\compile_commands.json" (
    echo ERROR: %BUILD_DIR%\compile_commands.json not found.
    exit /b 1
)

echo.
echo ==========================================
echo Fixing source files
echo ==========================================
echo.

for /R Source %%f in (*.cpp *.cxx *.cc) do (
    echo.
    echo ===== FIXING %%f =====
    clang-tidy "%%f" -p "%BUILD_DIR%" --fix
)

echo.
echo ==========================================
echo Fixing header files
echo ==========================================
echo.

for /R Include %%f in (*.h *.hpp *.hh) do (
    echo.
    echo ===== FIXING %%f =====
    clang-tidy "%%f" -p "%BUILD_DIR%" --fix
)

echo.
echo Done.

