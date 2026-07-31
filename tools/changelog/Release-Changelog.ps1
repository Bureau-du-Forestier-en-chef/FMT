<#
.SYNOPSIS
    Publie une nouvelle version a partir du CHANGELOG : commit, tag, branche d'archivage, push.

.DESCRIPTION
    A lancer APRES avoir colle la reponse de Copilot dans CHANGELOG.fr.md / CHANGELOG.md
    et SAUVEGARDE les fichiers. Le script :
      A. verifie que le changelog a change depuis le dernier commit ;
      B. lit le titre du haut de CHANGELOG.md :
         - "## [vX.Y.Z]"    -> mode VERSION (commit + tag + branche + bump + push) ;
         - "## [Unreleased]" -> mode ACCUMULATION (commit + push seulement, aucun tag).
      C. en mode version : valide (version > dernier tag ; tag et branche pas deja existants) ;
      D. apres confirmation : git add -A, commit, puis (mode version) tag, branche
         archive/vX.Y.Z si mineure+, bump CMakeLists/vcpkg, puis push origin.
    Aucune action git n'est faite sans la confirmation [o/N].

.PARAMETER Push
    Pousse commit + tag + branche vers origin (defaut : oui). -Push:$false garde tout local.

.PARAMETER DryRun
    Affiche le plan et s'arrete sans rien executer ni demander de confirmation.
#>
[CmdletBinding()]
param(
    [bool]$Push = $true,
    [switch]$DryRun
)

$ErrorActionPreference = 'Stop'
try { [Console]::OutputEncoding = [System.Text.Encoding]::UTF8 } catch { }

function Run-Git([string[]]$a) {
    & git @a
    if ($LASTEXITCODE -ne 0) { throw "Echec de : git $($a -join ' ')" }
}

# Synchronise le numero de version (X.Y.Z) dans un fichier, en preservant les octets
# (lecture/ecriture Latin-1 : mapping octet<->char sans perte, ne corrompt pas le cp1252).
# Le motif doit exposer 2 groupes nommes : 'pre' (conserve) et 'ver' (remplace).
# Renvoie l'ancienne version, ou $null si fichier/motif absent.
function Sync-VersionFile {
    param(
        [string]$Path,
        [string]$PatternStr,
        [string]$NewVersion,
        [bool]$Apply,
        [int]$MaxReplace = 0
    )
    if (-not (Test-Path $Path)) { return $null }
    $enc   = [System.Text.Encoding]::GetEncoding(28591)   # ISO-8859-1 / Latin-1
    $text  = $enc.GetString([System.IO.File]::ReadAllBytes($Path))
    $re    = [regex]$PatternStr
    $m     = $re.Match($text)
    if (-not $m.Success) {
        Write-Host "  ! $Path : motif de version introuvable, non synchronise." -ForegroundColor Yellow
        return $null
    }
    $old = $m.Groups['ver'].Value
    if ($Apply -and $old -ne $NewVersion) {
        $replacement = '${pre}' + $NewVersion
        if ($MaxReplace -gt 0) { $new = $re.Replace($text, $replacement, $MaxReplace) }
        else                   { $new = $re.Replace($text, $replacement) }
        [System.IO.File]::WriteAllBytes($Path, $enc.GetBytes($new))
    }
    return $old
}

# Racine du depot.
$repoRoot = (& git rev-parse --show-toplevel 2>$null)
if (-not $repoRoot) { Write-Error "Ce dossier n'est pas un depot git."; exit 1 }
Set-Location $repoRoot

$changelog = 'CHANGELOG.md'
if (-not (Test-Path $changelog)) { Write-Error "$changelog introuvable."; exit 1 }

# A. Le changelog a-t-il change (non committe) ?
$changed = & git status --porcelain -- CHANGELOG.md CHANGELOG.fr.md
if (-not $changed) {
    Write-Host "Le changelog n'a pas change depuis le dernier commit : rien a publier." -ForegroundColor Yellow
    Write-Host "Collez la reponse de Copilot dans les changelogs et sauvegardez d'abord." -ForegroundColor Yellow
    exit 0
}
$fileList = @($changed)
if ($fileList.Count -eq 1) {
    Write-Host "Note : un seul des deux changelogs semble modifie. Verifiez fr + en." -ForegroundColor Yellow
}

# B. Lire le TOUT PREMIER titre de section : ## [ ... ]  (version ou Unreleased).
$firstHeading = Select-String -Path $changelog -Pattern '^\s{0,3}##\s*\[(.+?)\]' | Select-Object -First 1
if (-not $firstHeading) {
    Write-Error "Aucun titre de section '## [...]' trouve en haut de $changelog."
    exit 1
}
$label = $firstHeading.Matches[0].Groups[1].Value.Trim()

$isUnreleased   = $false
$version        = $null
$numericVersion = $null
if ($label -match '^(unreleased|non[\s-]*publi|in[\s-]*dev|in[\s-]*progress|wip)') {
    # Accumulation : on commit/pousse le changelog, mais pas de tag/branche/bump.
    $isUnreleased = $true
}
elseif ($label -match '^v?(\d+)\.(\d+)\.(\d+)$') {
    $maj = [int]$Matches[1]; $min = [int]$Matches[2]; $pat = [int]$Matches[3]
    $version = "v$maj.$min.$pat"
    $numericVersion = "$maj.$min.$pat"   # sans le prefixe 'v', pour CMake / vcpkg
}
else {
    Write-Error "Le titre du haut '[$label]' n'est ni une version [vX.Y.Z] ni [Unreleased]. Corrigez-le."
    exit 1
}

# Fichiers ou synchroniser le numero de version (mode version seulement).
$versionFiles = @(
    @{ Path = 'CMakeLists.txt'; Pattern = '(?<pre>project\s*\(\s*FMT\s+VERSION\s+)(?<ver>\d+(?:\.\d+)*)'; Max = 0 }
    @{ Path = 'vcpkg.json';     Pattern = '(?<pre>"version"\s*:\s*")(?<ver>\d+(?:\.\d+)*)';               Max = 1 }
)

$currentBranch = (& git rev-parse --abbrev-ref HEAD).Trim()
$isMinorPlus   = $false
$archiveBranch = $null
$prevTag       = $null

if (-not $isUnreleased) {
    # Version precedente (dernier tag).
    $prevTag = (& git describe --tags --abbrev=0 2>$null)
    $pMaj = 0; $pMin = 0; $pPat = 0
    if ($prevTag -match 'v?(\d+)\.(\d+)\.(\d+)') {
        $pMaj = [int]$Matches[1]; $pMin = [int]$Matches[2]; $pPat = [int]$Matches[3]
    }

    # C. Validations.
    $newNum = 1000000 * $maj + 1000 * $min + $pat
    $oldNum = 1000000 * $pMaj + 1000 * $pMin + $pPat
    if ($prevTag -and $newNum -le $oldNum) {
        Write-Error "Version $version <= dernier tag $prevTag. Corrigez le titre du changelog."
        exit 1
    }

    # Mineure ou plus = changement au-dela du dernier chiffre (patch).
    $isMinorPlus   = ($maj -gt $pMaj) -or ($min -gt $pMin)
    $archiveBranch = "archive/$version"

    # Le tag ne doit pas exister deja.
    & git rev-parse -q --verify "refs/tags/$version" > $null 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Error "Le tag $version existe deja. Abandon (incrementez la version)."
        exit 1
    }
    # La branche d'archivage ne doit pas exister deja (locale ni distante).
    if ($isMinorPlus) {
        & git rev-parse -q --verify "refs/heads/$archiveBranch" > $null 2>&1
        $localExists = ($LASTEXITCODE -eq 0)
        $remoteExists = [bool](& git ls-remote --heads origin $archiveBranch 2>$null)
        if ($localExists -or $remoteExists) {
            Write-Error "La branche $archiveBranch existe deja (locale ou distante). Abandon."
            exit 1
        }
    }
}

# Message de commit (demande ; defaut = version, ou libelle changelog en mode Unreleased).
$defaultMsg = if ($isUnreleased) { 'Mise a jour du changelog (Unreleased)' } else { $version }
$msg = $defaultMsg
if (-not $DryRun) {
    $entered = Read-Host "Message de commit (Entree = $defaultMsg)"
    if (-not [string]::IsNullOrWhiteSpace($entered)) { $msg = $entered }
}

# Plan.
Write-Host ""
Write-Host "==================== PLAN DE PUBLICATION ====================" -ForegroundColor Cyan
if ($isUnreleased) {
    Write-Host "  Mode        : Unreleased -> commit + push seulement (PAS de tag/branche/bump)." -ForegroundColor Yellow
} else {
    Write-Host ("  Version     : {0}   (precedente : {1})" -f $version, $(if ($prevTag) { $prevTag } else { '(aucune)' }))
    Write-Host ("  Type        : {0}" -f $(if ($isMinorPlus) { "mineure/majeure -> branche $archiveBranch" } else { 'patch -> pas de branche' }))
}
Write-Host ("  Branche     : {0}" -f $currentBranch)
Write-Host ("  Message     : {0}" -f $msg)
Write-Host ("  Push origin : {0}" -f $(if ($Push) { 'oui' } else { 'non (tout reste local)' }))
if (-not $isUnreleased) {
    Write-Host "  Fichiers de version (mis a jour avant le commit) :"
    foreach ($vf in $versionFiles) {
        $old = Sync-VersionFile -Path $vf.Path -PatternStr $vf.Pattern -NewVersion $numericVersion -Apply $false -MaxReplace $vf.Max
        if ($null -ne $old) {
            $state = if ($old -eq $numericVersion) { "deja a $numericVersion" } else { "$old -> $numericVersion" }
            Write-Host "     $($vf.Path) : $state"
        }
    }
}
Write-Host "  A committer (git add -A) :"
& git status --short | ForEach-Object { Write-Host "     $_" }
Write-Host ""
Write-Host "  Actions :"
if (-not $isUnreleased) { Write-Host "    maj version -> $numericVersion dans CMakeLists.txt / vcpkg.json" }
Write-Host "    git add -A"
Write-Host "    git commit -m `"$msg`""
if (-not $isUnreleased) { Write-Host "    git tag -a $version -m `"$msg`"" }
if ($isMinorPlus)       { Write-Host "    git branch $archiveBranch" }
if ($Push) {
    Write-Host "    git push origin $currentBranch"
    if (-not $isUnreleased) { Write-Host "    git push origin $version" }
    if ($isMinorPlus)       { Write-Host "    git push origin $archiveBranch" }
}
Write-Host "============================================================" -ForegroundColor Cyan

if ($DryRun) {
    Write-Host "(DryRun) Aucune action git executee." -ForegroundColor Yellow
    exit 0
}

$confirm = Read-Host "Confirmer ? [o/N]"
if ($confirm -ne 'o' -and $confirm -ne 'O') {
    Write-Host "Annule. Aucune action git effectuee." -ForegroundColor Yellow
    exit 0
}

# D. Execution.
if (-not $isUnreleased) {
    foreach ($vf in $versionFiles) {
        Sync-VersionFile -Path $vf.Path -PatternStr $vf.Pattern -NewVersion $numericVersion -Apply $true -MaxReplace $vf.Max | Out-Null
    }
}
Run-Git @('add', '-A')
Run-Git @('commit', '-m', $msg)
if (-not $isUnreleased) { Run-Git @('tag', '-a', $version, '-m', $msg) }
if ($isMinorPlus)       { Run-Git @('branch', $archiveBranch) }
if ($Push) {
    Run-Git @('push', 'origin', $currentBranch)
    if (-not $isUnreleased) { Run-Git @('push', 'origin', $version) }
    if ($isMinorPlus)       { Run-Git @('push', 'origin', $archiveBranch) }
}

Write-Host ""
if ($isUnreleased) {
    Write-Host "Changelog (Unreleased) committe et pousse sur $currentBranch. Aucun tag cree." -ForegroundColor Green
} else {
    Write-Host "Publication terminee : $version" -ForegroundColor Green
    if ($isMinorPlus) {
        Write-Host "Branche d'archivage creee : $archiveBranch (vous restez sur $currentBranch)." -ForegroundColor Green
    }
}
