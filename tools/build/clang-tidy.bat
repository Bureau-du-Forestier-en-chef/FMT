@echo off
setlocal EnableDelayedExpansion

pushd "%~dp0..\.."

set BUILD_DIR=build\clang
set FAILED=0

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

for /R Source %%f in (*.cpp) do (

    echo.
    echo ===== %%f =====

    clang-tidy "%%f" -p "%BUILD_DIR%" --quiet

    if errorlevel 1 (
        echo.
        echo ERROR while processing %%f
        set FAILED=1
    )
)

for /R UI\Source %%f in (*.cpp) do (

    echo.
    echo ===== %%f =====

    clang-tidy "%%f" -p "%BUILD_DIR%" --quiet

    if errorlevel 1 (
        echo.
        echo ERROR while processing %%f
        set FAILED=1
    )
)

for /R Excel\Source %%f in (*.cpp) do (

    echo.
    echo ===== %%f =====

    clang-tidy "%%f" -p "%BUILD_DIR%" --quiet

    if errorlevel 1 (
        echo.
        echo ERROR while processing %%f
        set FAILED=1
    )
)

popd

echo.

if "!FAILED!"=="1" (
    echo ==========================================
    echo CLANG-TIDY FAILED
    echo ==========================================
    exit /b 1
)

echo ==========================================
echo CLANG-TIDY PASSED
echo ==========================================
exit /b 0
