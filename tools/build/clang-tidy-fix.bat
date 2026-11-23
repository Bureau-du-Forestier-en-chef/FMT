@echo off
setlocal

call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"

pushd "%~dp0..\.."

set BUILD_DIR=build\clang

if not exist "%BUILD_DIR%\compile_commands.json" (
    echo ERROR: %BUILD_DIR%\compile_commands.json not found.
    pause
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
    clang-tidy "%%f" -p "%BUILD_DIR%" --fix --quiet
)

echo.

echo ==========================================
echo Rebuilding project
echo ==========================================
echo.


cmake --build "%BUILD_DIR%"


if errorlevel 1 (
    echo.
    echo BUILD FAILED AFTER CLANG-TIDY FIXES
    pause
    exit /b 1
)

echo.
echo ==========================================
echo SUCCESS
echo ==========================================
echo.

popd
pause


