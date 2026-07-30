@echo off
setlocal EnableExtensions
REM ============================================================================
REM  Changelog_Generate.bat
REM  Genere un prompt (Conventional Commits / Keep a Changelog) a partir des
REM  commits depuis le dernier tag, et le copie dans le presse-papier pour le
REM  coller dans Copilot (Teams / app Windows).
REM
REM  Au lancement, choisissez le contenu a envoyer a Copilot :
REM    [1] messages de commit seuls  (defaut)
REM    [2] + diff complet du code    (recommande si peu de commits)
REM
REM  Argument optionnel : une reference de depart (tag / hash / date).
REM    Changelog_Generate.bat            (depuis le dernier tag)
REM    Changelog_Generate.bat v1.0.2     (depuis une reference precise)
REM ============================================================================

set "DIFFOPT="
set "CHOICE="
echo.
echo   Contenu a envoyer a Copilot :
echo     [1] Messages de commit seuls  (defaut)
echo     [2] + diff complet du code    (recommande si peu de commits)
echo.
set /p "CHOICE=  Votre choix [1/2] (Entree = 1) : "
if "%CHOICE%"=="2" set "DIFFOPT=-Diff"

echo.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\changelog\Generate-Changelog.ps1" %* %DIFFOPT%

endlocal
pause
