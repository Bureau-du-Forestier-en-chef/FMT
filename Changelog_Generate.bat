@echo off
setlocal EnableExtensions
REM ============================================================================
REM  Changelog_Generate.bat
REM  1) Genere un prompt (commits + fichiers non commites + issues resolues) et le
REM     copie dans le presse-papier pour le coller dans Copilot (Teams / app Windows).
REM  2) Pause : vous collez les deux blocs changelog dans CHANGELOG.fr.md /
REM     CHANGELOG.md, verifiez le numero de version et SAUVEGARDEZ.
REM  3) Publication (apres confirmation) : commit (code + changelog ensemble) et,
REM     si le titre est une version, tag + branche d'archivage + push origin.
REM     Si le titre est [Unreleased] : commit + push seulement, sans tag.
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
echo     [1] Commits + fichiers non commites   (defaut)
echo     [2] + diff complet du code            (recommande si peu de commits)
echo.
set /p "CHOICE=  Votre choix [1/2] (Entree = 1) : "
if "%CHOICE%"=="2" set "DIFFOPT=-Diff"

echo.
%PS% "%DIR%\Generate-Changelog.ps1" %* %DIFFOPT%

echo.
echo ============================================================
echo   ETAPES :
echo     1. Collez (Ctrl+V) le prompt dans Copilot (Teams / app) et envoyez.
echo     2. La reponse contient TROIS blocs. Collez les deux premiers :
echo          - bloc FRANCAIS -^> en haut de CHANGELOG.fr.md
echo          - bloc ANGLAIS  -^> en haut de CHANGELOG.md
echo        (gardez le 3e, le MESSAGE DE COMMIT, sous la main pour l'etape 4)
echo     3. Verifiez le numero de version propose, puis SAUVEGARDEZ les deux fichiers.
echo        Titre [vX.Y.Z] = version taguee ; [Unreleased] = simple accumulation.
echo     4. Revenez ici et appuyez sur une touche. Collez le message de commit a
echo        l'invite (ou Entree pour celui deduit), puis confirmez : le script commite,
echo        et si c'est une version, tague / archive / pousse. Fermez pour annuler.
echo ============================================================
pause

echo.
%PS% "%DIR%\Release-Changelog.ps1"

endlocal
echo.
pause
