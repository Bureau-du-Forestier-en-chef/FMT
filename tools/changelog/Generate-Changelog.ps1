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

# Recupere les issues fermees depuis une date, puis ne GARDE que celles fermees
# comme "completed" (reellement resolues ; les "not planned" = doublons/invalides/
# abandonnees sont ecartees). Source : gh d'abord, sinon l'API REST publique (sans auth).
# Comparaison de state_reason insensible a la casse (API renvoie 'completed',
# gh renvoie 'COMPLETED'). Renvoie { Issues=@(); Source; RawTotal; Fetched }.
function Get-ClosedIssues {
    param([string]$OwnerRepo, [string]$SinceDate)
    $res = [pscustomobject]@{ Issues = @(); Source = 'none'; RawTotal = 0; Fetched = 0 }
    if (-not $OwnerRepo) { return $res }

    # 1) gh issue list (si gh present et connecte)
    if (Get-Command gh -ErrorAction SilentlyContinue) {
        try {
            $ghArgs = @('issue', 'list', '--repo', $OwnerRepo, '--state', 'closed', '--limit', '300', '--json', 'number,title,stateReason')
            if ($SinceDate) { $ghArgs += @('--search', "closed:>=$SinceDate") }
            $out = & gh @ghArgs 2>$null
            if ($LASTEXITCODE -eq 0 -and $out) {
                $all = @($out | ConvertFrom-Json)
                $res.Fetched  = $all.Count
                $res.RawTotal = $all.Count
                $res.Issues   = @($all | Where-Object { $_.stateReason -eq 'completed' } |
                                  ForEach-Object { [pscustomobject]@{ number = $_.number; title = $_.title } })
                $res.Source   = 'gh'
                return $res
            }
        } catch { }
    }

    # 2) API REST publique de GitHub (repli, sans auth ; ~10 req/min)
    try {
        $q = "repo:$OwnerRepo is:issue is:closed"
        if ($SinceDate) { $q += " closed:>=$SinceDate" }
        $uri = "https://api.github.com/search/issues?q=" + [uri]::EscapeDataString($q) + "&per_page=100"
        $headers = @{ 'User-Agent' = 'FMT-changelog-tool'; 'Accept' = 'application/vnd.github+json' }
        $resp = Invoke-RestMethod -Uri $uri -Headers $headers -Method Get -TimeoutSec 20
        $items = @($resp.items)
        $res.Fetched  = $items.Count
        $res.RawTotal = [int]$resp.total_count
        $res.Issues   = @($items | Where-Object { $_.state_reason -eq 'completed' } |
                          ForEach-Object { [pscustomobject]@{ number = $_.number; title = $_.title } })
        $res.Source   = 'api'
        return $res
    } catch { }

    return $res
}

# Renvoie la premiere section d'un changelog si son titre est [Unreleased] (ou un
# equivalent : Non publie, in dev, in progress, WIP), du titre jusqu'a la section
# suivante ; $null sinon. Lit le fichier sur disque, retouches non commitees comprises.
function Get-UnreleasedSection {
    param([string]$Path)
    if (-not (Test-Path $Path)) { return $null }
    $text = Get-Content -Raw -Encoding UTF8 $Path
    if (-not $text) { return $null }
    $m = [regex]::Match($text, '(?ms)^[ \t]{0,3}##[ \t]*\[(?<label>[^\]\r\n]+)\][^\r\n]*\r?\n.*?(?=^[ \t]{0,3}##[ \t]*\[|\z)')
    if (-not $m.Success) { return $null }
    if ($m.Groups['label'].Value.Trim() -notmatch '^(unreleased|non[\s-]*publi|in[\s-]*dev|in[\s-]*progress|wip)') { return $null }
    return $m.Value.TrimEnd()
}

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

# URL du depot (derivee de origin, avec repli sur l'URL connue de FMT).
$remote  = (& git remote get-url origin 2>$null)
$repoUrl = $null
if ($remote) {
    $r = $remote.Trim()
    if     ($r -match '^git@([^:]+):(.+?)(?:\.git)?$')     { $repoUrl = "https://$($Matches[1])/$($Matches[2])" }
    elseif ($r -match '^(https?://[^/]+)/(.+?)(?:\.git)?$') { $repoUrl = "$($Matches[1])/$($Matches[2])" }
}
if (-not $repoUrl) { $repoUrl = 'https://github.com/Bureau-du-Forestier-en-chef/FMT' }

# Date de la version de base, pour filtrer les issues fermees depuis.
$baseDate = ''
if ($Since) { 
    # On tente d'abord de lire comme un tag exact, sinon on laisse Git chercher (ex: si c'est un commit hash ou HEAD~3)
    $refToCheck = if (& git show-ref --tags $Since 2>$null) { "refs/tags/$Since" } else { $Since }
    $baseDate = (& git log -1 --format=%ad --date=short $refToCheck 2>$null) 
}
if ($baseDate) { $baseDate = $baseDate.Trim() }
$baseDateDesc = if ($baseDate) { $baseDate } else { '(debut de l''historique)' }

# URL de recherche GitHub : issues fermees depuis la base.
$issueQuery = 'is:issue is:closed'
if ($baseDate) { $issueQuery += " closed:>=$baseDate" }
$issuesUrl = "$repoUrl/issues?q=" + ([uri]::EscapeDataString($issueQuery)).Replace('%20', '+')

# Issues fermees comme "completed" (resolues) depuis la base, injectees dans le prompt.
$ownerRepo = $repoUrl -replace '^https?://[^/]+/', ''
$issuesData = Get-ClosedIssues -OwnerRepo $ownerRepo -SinceDate $baseDate
$issueCount = $issuesData.Issues.Count
$closedIssuesBlock = ''
if ($issuesData.Source -ne 'none') {
    $srcLabel = if ($issuesData.Source -eq 'gh') { 'gh issue list' } else { 'API GitHub' }
    $note = ''
    if ($issuesData.RawTotal -gt $issuesData.Fetched) {
        $note = " ; NB: 1re page seulement ($($issuesData.Fetched)/$($issuesData.RawTotal)), voir $issuesUrl"
    }
    $header = "ISSUES FERMEES ET RESOLUES (completed) depuis $baseDateDesc (source: $srcLabel ; $issueCount resolues sur $($issuesData.RawTotal) fermees)$note :"
    if ($issueCount -gt 0) {
        $lines = $issuesData.Issues | ForEach-Object { "- #$($_.number) $($_.title)" }
        $closedIssuesBlock = "`r`n`r`n$header`r`n" + ($lines -join "`r`n")
    } else {
        $closedIssuesBlock = "`r`n`r`n$header`r`n(aucune)"
    }
}

# Modifications non commitees de l'arbre de travail, changelogs exclus (sinon
# le prompt decrirait sa propre sortie).
$wtPathspec = @('--', '.', ':(exclude)CHANGELOG.md', ':(exclude)CHANGELOG.fr.md')
$wtStatus   = @(& git status --porcelain | Where-Object { $_ -notmatch 'CHANGELOG(\.fr)?\.md$' })
$wtCount    = $wtStatus.Count

# Section [Unreleased] deja presente en tete des changelogs. Copilot la reprend et renvoie
# UNE section consolidee qui la remplace, au lieu d'en empiler une seconde.
$existingEn  = Get-UnreleasedSection -Path 'CHANGELOG.md'
$existingFr  = Get-UnreleasedSection -Path 'CHANGELOG.fr.md'
$hasExisting = [bool]($existingEn -or $existingFr)

# Repere des commits deja decrits : le dernier commit qui a modifie un changelog. Le hash
# ecrit dans le titre [Unreleased] ne convient pas : c'est le HEAD au moment de la
# generation, et la section est commitee dans le commit SUIVANT avec le code qu'elle decrit.
$anchor = $null
if ($hasExisting -and $Since) {
    $lastClCommit = (& git log -1 --format=%h -- CHANGELOG.md CHANGELOG.fr.md 2>$null)
    if ($lastClCommit) {
        $lastClCommit = $lastClCommit.Trim()
        $afterBase = (& git rev-list --count "$Since..$lastClCommit" 2>$null)
        if ($afterBase -and [int]$afterBase -gt 0) { $anchor = $lastClCommit }
    }
}

# Recuperer les commits (hors merges). La plage part toujours de la base : rien n'est oublie.
$logFormat = '--pretty=format:- %s (%h, %an, %ad)'
$commits = @(& git log $range --no-merges $logFormat --date=short | Where-Object { $_ })
$count   = $commits.Count
if ($count -eq 0 -and $wtCount -eq 0) {
    Write-Host "Rien a documenter : aucun commit sur '$range' et aucun changement non commite." -ForegroundColor Yellow
    exit 0
}
$newCount = $count
$oldCount = 0
if ($anchor) {
    $newCommits = @(& git log "$anchor..HEAD" --no-merges $logFormat --date=short | Where-Object { $_ })
    $oldCommits = @(& git log "$Since..$anchor" --no-merges $logFormat --date=short | Where-Object { $_ })
    $newCount = $newCommits.Count
    $oldCount = $oldCommits.Count
    $newText  = if ($newCount) { $newCommits -join "`r`n" } else { '(aucun)' }
    $oldText  = if ($oldCount) { $oldCommits -join "`r`n" } else { '(aucun)' }
    $commitsText = "-- NOUVEAUX depuis $anchor (a integrer) : $newCount --`r`n$newText`r`n`r`n" +
                   "-- DEJA DECRITS dans la section non publiee ($Since..$anchor, contexte seulement) : $oldCount --`r`n$oldText"
} else {
    $commitsText = if ($count) { $commits -join "`r`n" } else { '(aucun nouveau commit depuis la base)' }
}

# Bloc de la section non publiee existante, a remplacer par la reponse de Copilot.
$existingBlock = ''
if ($hasExisting) {
    $exParts = @('SECTION NON PUBLIEE EXISTANTE (ta reponse la REMPLACE) :')
    if ($existingEn) { $exParts += @('', '--- actuellement en tete de CHANGELOG.md ---', $existingEn) }
    if ($existingFr) { $exParts += @('', '--- actuellement en tete de CHANGELOG.fr.md ---', $existingFr) }
    $existingBlock = "`r`n`r`n" + ($exParts -join "`r`n")
}

# Budget de diff partage : le travail non commite est PRIORITAIRE (aucun message de
# commit ne le decrit), le diff des commits prend ce qui reste.
$budget = $MaxDiffChars

# Section MODIFICATIONS NON COMMITEES.
$workingTreeBlock = ''
$wtDiffChars = 0
if ($wtCount -gt 0) {
    $wtStat = (& git diff HEAD --stat @wtPathspec | Out-String).TrimEnd()
    $parts  = @("MODIFICATIONS NON COMMITEES (arbre de travail, changelogs exclus) - $wtCount entree(s) :",
                ($wtStatus -join "`r`n"))
    if ($wtStat) { $parts += @('', 'Volume par fichier :', $wtStat) }
    if ($Diff) {
        $wtFull = (& git diff HEAD @wtPathspec | Out-String).TrimEnd()
        $wtDiffChars = $wtFull.Length
        if ($wtDiffChars -gt $budget) {
            $wtFull = $wtFull.Substring(0, $budget) +
                      "`r`n`r`n... [diff non commite tronque a $budget caracteres] ..."
            $budget = 0
        } else {
            $budget -= $wtDiffChars
        }
        if ($wtFull) { $parts += @('', 'DIFF complet des changements non commites :', $wtFull) }
    }
    $workingTreeBlock = "`r`n`r`n" + ($parts -join "`r`n")
}

# Section DIFF des commits (optionnelle, sur le budget restant). Avec une section non
# publiee existante, seuls les NOUVEAUX commits sont diffes : le reste est deja decrit.
$diffBase  = if ($anchor) { $anchor } else { $Since }
$diffBlock = ''
$diffChars = 0
$diffNote  = ''
if ($Diff) {
    if (-not $diffBase) {
        $diffNote = 'omis (aucune reference de base)'
    } elseif ($anchor -and $newCount -eq 0) {
        $diffNote = 'aucun nouveau commit depuis la section non publiee'
    } elseif ($budget -le 0) {
        $diffNote = 'omis (plafond consomme par le diff non commite)'
    } else {
        $stat = (& git diff --stat $diffBase HEAD | Out-String).TrimEnd()
        $full = (& git diff $diffBase HEAD | Out-String).TrimEnd()
        $diffChars = $full.Length
        if ($diffChars -gt $budget) {
            $full = $full.Substring(0, $budget) +
                    "`r`n`r`n... [diff tronque a $budget caracteres - restreignez la plage] ..."
        }
        $diffBlock = "`r`n`r`nDIFF DES COMMITS (plage $diffBase..HEAD) - resume :`r`n$stat`r`n`r`nDIFF complet :`r`n$full"
    }
}

# Charger le gabarit et substituer les placeholders.
$templatePath = Join-Path $PSScriptRoot 'prompt-template.txt'
if (-not (Test-Path $templatePath)) {
    Write-Error "Gabarit introuvable : $templatePath"
    exit 1
}
$template = Get-Content -Raw -Encoding UTF8 $templatePath
$values = @{
    RANGE               = $range
    BASE                = $baseDesc
    BASE_DATE           = $baseDateDesc
    DATE                = $today
    HEAD                = $headHash
    REPO_URL            = $repoUrl
    ISSUES_URL          = $issuesUrl
    COMMITS             = $commitsText
    DIFF                = $diffBlock
    WORKING_TREE        = $workingTreeBlock
    CLOSED_ISSUES       = $closedIssuesBlock
    EXISTING_UNRELEASED = $existingBlock
}
# Substitution en UNE passe sur le gabarit seul : une valeur inseree n'est jamais relue.
# Des .Replace() en chaine remplacaient aussi les jetons presents DANS les donnees deja
# inserees (ex. le diff de l'outil lui-meme), ce qui corrompait le prompt.
$sb  = New-Object System.Text.StringBuilder
$pos = 0
foreach ($tok in [regex]::Matches($template, '\{\{([A-Z_]+)\}\}')) {
    [void]$sb.Append($template, $pos, $tok.Index - $pos)
    $key = $tok.Groups[1].Value
    if ($values.ContainsKey($key)) { [void]$sb.Append([string]$values[$key]) } else { [void]$sb.Append($tok.Value) }
    $pos = $tok.Index + $tok.Length
}
[void]$sb.Append($template, $pos, $template.Length - $pos)
$prompt = $sb.ToString()

# Copier dans le presse-papier.
Set-Clipboard -Value $prompt

# Consignes (ASCII pour rester lisible dans toutes les consoles).
Write-Host ""
Write-Host "==================================================================" -ForegroundColor Cyan
Write-Host "  Prompt CHANGELOG copie dans le presse-papier." -ForegroundColor Green
Write-Host "  Plage : $range  ($count commits)  base=$baseDesc  HEAD=$headHash" -ForegroundColor Green
if ($wtCount -gt 0) {
    $wtNote = if ($Diff -and $wtDiffChars) { " (diff joint : $wtDiffChars caracteres)" } else { ' (liste + volume par fichier)' }
    Write-Host "  Non commite : $wtCount entree(s) de l'arbre de travail injectees$wtNote." -ForegroundColor Yellow
    Write-Host "                -> elles seront commitees AVEC le changelog qui les decrit." -ForegroundColor Yellow
} else {
    Write-Host "  Non commite : rien (arbre de travail propre)." -ForegroundColor DarkGray
}
if ($hasExisting) {
    if ($anchor) {
        Write-Host "  Section [Unreleased] existante : $oldCount commit(s) deja decrit(s) (jusqu'a $anchor), $newCount nouveau(x)." -ForegroundColor Yellow
    } else {
        Write-Host "  Section [Unreleased] existante : injectee dans le prompt." -ForegroundColor Yellow
    }
    if ($anchor -and $newCount -eq 0 -and $wtCount -eq 0) {
        Write-Host "                -> rien de nouveau : la reponse la re-consolide (ex. pour la promouvoir en version)." -ForegroundColor Yellow
    } else {
        Write-Host "                -> la reponse la REMPLACE : ne collez pas un second bloc par-dessus." -ForegroundColor Yellow
    }
}
if ($Diff -and $diffChars) {
    Write-Host "  Diff des commits joint : $diffChars caracteres (plage $diffBase..HEAD, plafond global $MaxDiffChars partage)." -ForegroundColor Green
} elseif ($Diff) {
    Write-Host "  Diff des commits : $diffNote." -ForegroundColor Yellow
} else {
    Write-Host "  (sans diff du code : messages de commit + liste des fichiers)" -ForegroundColor DarkGray
}
if ($issuesData.Source -eq 'none') {
    Write-Host "  Issues : non recuperees (gh/API indispo) ; le prompt pointe vers :" -ForegroundColor DarkGray
    Write-Host "    $issuesUrl" -ForegroundColor DarkGray
} else {
    Write-Host "  Issues resolues (completed) depuis $baseDateDesc : $issueCount injectees sur $($issuesData.RawTotal) fermees (source: $($issuesData.Source))." -ForegroundColor Green
}
Write-Host "==================================================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "  -> Collez ce prompt dans Copilot. Sa reponse contient 3 blocs :" -ForegroundColor DarkGray
Write-Host "     changelog FR, changelog EN, et un message de commit a coller a l'invite." -ForegroundColor DarkGray
Write-Host ""
