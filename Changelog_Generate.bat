@echo off
setlocal EnableExtensions
REM ============================================================================
REM  Changelog_Generate.bat
REM  1) Genere un prompt (Conventional Commits / Keep a Changelog) et le copie
REM     dans le presse-papier pour le coller dans Copilot (Teams / app Windows).
REM  2) Pause : vous collez la reponse dans CHANGELOG.fr.md / CHANGELOG.md,
REM     verifiez le numero de version et SAUVEGARDEZ.
REM  3) Publication (apres confirmation) : commit, tag, branche d'archivage si
REM     version mineure ou plus, puis push origin.
REM
REM  Argument optionnel : une reference de depart (tag / hash / date).
REM    Changelog_Generate.bat            (depuis le dernier tag)
REM    Changelog_Generate.bat v1.0.2     (depuis une reference precise)
REM ============================================================================

set "DIR=%~dp0tools\changelog"
set "PS=powershell -NoProfile -ExecutionPolicy Bypass -File"

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
%PS% "%DIR%\Generate-Changelog.ps1" %* %DIFFOPT%

echo.
echo ============================================================
echo   ETAPES :
echo     1. Collez (Ctrl+V) le prompt dans Copilot (Teams / app) et envoyez.
echo     2. Copiez les DEUX blocs de la reponse :
echo          - bloc FRANCAIS -^> en haut de CHANGELOG.fr.md
echo          - bloc ANGLAIS  -^> en haut de CHANGELOG.md
echo     3. Verifiez le numero de version propose, puis SAUVEGARDEZ les deux fichiers.
echo     4. Revenez ici et appuyez sur une touche : apres confirmation, le script
echo        va committer, taguer, archiver (si mineure+) et pousser la version.
echo        Fermez la fenetre pour annuler.
echo ============================================================
pause

echo.
%PS% "%DIR%\Release-Changelog.ps1"

endlocal
echo.
pause
