@echo off
setlocal

pushd "%~dp0..\.."

call tools\build\clang-tidy-fix.bat

if errorlevel 1 (
    echo.
    echo clang-tidy failed.
    popd
    pause
    exit /b 1
)

echo.
echo ==========================================
echo Rebuilding project
echo ==========================================
echo.

cmake --build build\clang

if errorlevel 1 (
    echo.
    echo Build failed after clang-tidy fixes.
    popd
    pause
    exit /b 1
)

echo.
echo ==========================================
echo Validation successful.
echo ==========================================

popd
pause

