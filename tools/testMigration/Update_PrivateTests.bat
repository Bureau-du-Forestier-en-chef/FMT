@echo off
setlocal EnableExtensions
REM ============================================================================
REM  Update_PrivateTests.bat
REM  Renames the test executables in the private copy of BFECtests.csv of a
REM  worktree, after their renaming under #350 (Update-PrivateTests.ps1, with
REM  the table Tests\Support\RenamedTests.csv). The arguments go to the script:
REM    Update_PrivateTests.bat
REM    Update_PrivateTests.bat -Worktree C:\Users\me\Documents\FMT\dev
REM  Without argument, as from the Explorer: the worktree of this script, then
REM  a pause.
REM ============================================================================

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Update-PrivateTests.ps1" %*
set "CODE=%ERRORLEVEL%"
if "%~1"=="" pause
endlocal & exit /b %CODE%
