@echo off
setlocal

echo ============================================================
echo Installing LLVM / clang
echo ============================================================
echo.

where winget >nul 2>&1
if errorlevel 1 (
    echo ERROR: winget is not installed.
    exit /b 1
)

winget install LLVM.LLVM --accept-package-agreements --accept-source-agreements

if errorlevel 1 (
    echo LLVM installation failed.
    exit /b 1
)

echo.
echo ============================================================
echo Installation complete
echo ============================================================
echo.

echo Checking installation...
call clang-tidy --version
call clang-format --version

echo.
pause