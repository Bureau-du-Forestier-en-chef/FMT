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
    Replace('{{RANGE}}',     $range).
    Replace('{{BASE}}',      $baseDesc).
    Replace('{{BASE_DATE}}', $baseDateDesc).
    Replace('{{DATE}}',      $today).
    Replace('{{HEAD}}',      $headHash).
    Replace('{{REPO_URL}}',  $repoUrl).
    Replace('{{ISSUES_URL}}',$issuesUrl).
    Replace('{{COMMITS}}',   $commitsText).
    Replace('{{DIFF}}',      $diffBlock).
    Replace('{{CLOSED_ISSUES}}', $closedIssuesBlock)

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
if ($issuesData.Source -eq 'none') {
    Write-Host "  Issues : non recuperees (gh/API indispo) ; le prompt pointe vers :" -ForegroundColor DarkGray
    Write-Host "    $issuesUrl" -ForegroundColor DarkGray
} else {
    Write-Host "  Issues resolues (completed) depuis $baseDateDesc : $issueCount injectees sur $($issuesData.RawTotal) fermees (source: $($issuesData.Source))." -ForegroundColor Green
}
Write-Host "==================================================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "  -> Collez ce prompt dans Copilot ; sa reponse ira dans CHANGELOG.fr.md / CHANGELOG.md." -ForegroundColor DarkGray
Write-Host ""
