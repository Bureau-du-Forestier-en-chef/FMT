<#
.SYNOPSIS
    Prepare un prompt pret-a-coller pour generer/mettre a jour le CHANGELOG via Copilot.

.DESCRIPTION
    Rassemble les commits depuis le dernier tag (ou une reference donnee), les injecte
    dans le gabarit prompt-template.txt, copie le tout dans le presse-papier et affiche
    les consignes. N'ECRIT PAS dans le changelog et NE TAGUE PAS : l'utilisateur colle
    la reponse de Copilot a la main dans CHANGELOG.fr.md et CHANGELOG.md.

.PARAMETER Since
    Reference git de depart (tag, hash ou date). Defaut : dernier tag
    (git describe --tags --abbrev=0). Si aucun tag, tout l'historique est utilise.

.PARAMETER Diff
    Joint le diff complet du code (git diff <base> HEAD, avec --stat) au prompt, pour que
    Copilot analyse les vrais changements et pas seulement les messages de commit.
    A utiliser sur de PETITES plages (2-3 commits) : un diff complet peut etre enorme.

.PARAMETER MaxDiffChars
    Taille max du diff joint (caracteres). Au-dela, le diff est tronque avec un avis.
    Defaut : 60000.

.EXAMPLE
    .\Generate-Changelog.ps1
.EXAMPLE
    .\Generate-Changelog.ps1 v1.0.2
.EXAMPLE
    .\Generate-Changelog.ps1 HEAD~3 -Diff
#>
[CmdletBinding()]
param(
    [Parameter(Position = 0)]
    [string]$Since,

    [switch]$Diff,

    [int]$MaxDiffChars = 60000
)

$ErrorActionPreference = 'Stop'

# Affichage console en UTF-8.
try { [Console]::OutputEncoding = [System.Text.Encoding]::UTF8 } catch { }

# Se placer a la racine du depot (le script vit dans tools/changelog/).
$repoRoot = (& git rev-parse --show-toplevel 2>$null)
if (-not $repoRoot) {
    Write-Error "Ce dossier n'est pas un depot git."
    exit 1
}
Set-Location $repoRoot

# Determiner le point de depart.
if (-not $Since) {
    $Since = (& git describe --tags --abbrev=0 2>$null)
}
if ($Since) { $range = "$Since..HEAD" } else { $range = "HEAD" }

# Contexte version (valeurs reelles, pour eviter que Copilot les invente).
$today    = Get-Date -Format 'yyyy-MM-dd'
$headHash = (& git rev-parse --short HEAD).Trim()
$baseDesc = if ($Since) { $Since } else { '(debut de l''historique)' }

# Recuperer les commits (hors merges).
$commits = & git log $range --no-merges --pretty=format:'- %s (%h, %an, %ad)' --date=short
if (-not $commits) {
    Write-Host "Aucun commit a documenter pour la plage '$range'." -ForegroundColor Yellow
    exit 0
}
$count = ($commits | Measure-Object -Line).Lines
$commitsText = ($commits -join "`r`n")

# Section DIFF optionnelle.
$diffBlock = ''
$diffChars = 0
if ($Diff) {
    if (-not $Since) {
        Write-Host "Attention : -Diff sans reference de base ; diff omis. Precisez un tag/hash." -ForegroundColor Yellow
    } else {
        $stat = (& git diff --stat $Since HEAD | Out-String).TrimEnd()
        $full = (& git diff $Since HEAD | Out-String).TrimEnd()
        $diffChars = $full.Length
        if ($diffChars -gt $MaxDiffChars) {
            $full = $full.Substring(0, $MaxDiffChars) +
                    "`r`n`r`n... [diff tronque a $MaxDiffChars caracteres - restreignez la plage] ..."
        }
        $diffBlock = "`r`n`r`nDIFF (plage $range) - resume :`r`n$stat`r`n`r`nDIFF complet :`r`n$full"
    }
}

# Charger le gabarit et substituer les placeholders.
$templatePath = Join-Path $PSScriptRoot 'prompt-template.txt'
if (-not (Test-Path $templatePath)) {
    Write-Error "Gabarit introuvable : $templatePath"
    exit 1
}
$template = Get-Content -Raw -Encoding UTF8 $templatePath
$prompt = $template.
    Replace('{{RANGE}}',   $range).
    Replace('{{BASE}}',    $baseDesc).
    Replace('{{DATE}}',    $today).
    Replace('{{HEAD}}',    $headHash).
    Replace('{{COMMITS}}', $commitsText).
    Replace('{{DIFF}}',    $diffBlock)

# Copier dans le presse-papier.
Set-Clipboard -Value $prompt

# Consignes (ASCII pour rester lisible dans toutes les consoles).
Write-Host ""
Write-Host "==================================================================" -ForegroundColor Cyan
Write-Host "  Prompt CHANGELOG copie dans le presse-papier." -ForegroundColor Green
Write-Host "  Plage : $range  ($count commits)  base=$baseDesc  HEAD=$headHash" -ForegroundColor Green
if ($Diff -and $diffChars) {
    Write-Host "  Diff joint : $diffChars caracteres (max $MaxDiffChars)." -ForegroundColor Green
} elseif (-not $Diff) {
    Write-Host "  (messages de commit seuls)" -ForegroundColor DarkGray
}
Write-Host "==================================================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "  Etapes :"
Write-Host "   1. Ouvrez Copilot (onglet Teams ou l'app Copilot Windows)."
Write-Host "   2. Collez (Ctrl+V) et envoyez."
Write-Host "   3. Copiez les deux blocs rendus par Copilot :"
Write-Host "        - bloc FRANCAIS -> collez-le en haut de CHANGELOG.fr.md"
Write-Host "        - bloc ANGLAIS  -> collez-le en haut de CHANGELOG.md"
Write-Host "      Chaque bloc contient deja un titre '## [vX.Y.Z] - $today ($headHash)'."
Write-Host "   4. Verifiez le numero propose ; ajustez-le au besoin, puis taguez :"
Write-Host "        git tag -a vX.Y.Z -m `"...`""
Write-Host ""
