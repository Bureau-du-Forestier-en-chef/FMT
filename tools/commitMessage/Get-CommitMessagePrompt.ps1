Add-Type -AssemblyName System.Windows.Forms

# Repository root (FMT)
$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")

Set-Location $RepoRoot

# Get staged diff
$diff = git diff --cached

if (-not $diff)
{
    [System.Windows.Forms.MessageBox]::Show(
        "No staged changes found.",
        "Commit Message Generator"
    )
    exit
}

# Get staged files
$files = git diff --cached --name-status

$prompt = @"
Generate a Conventional Commit message following:
https://www.conventionalcommits.org/en/v1.0.0/

Requirements:
- Determine the most appropriate type (feat, fix, build, refactor, docs, test, chore, ci, perf).
- Use a scope when appropriate.
- Subject line should be concise (<72 chars).
- Include a body when useful.
- Output ONLY the final commit message.

===== STAGED FILES =====

$files

===== STAGED DIFF =====

$diff

"@

Set-Clipboard -Value $prompt

Write-Host ""
Write-Host "Repository: $RepoRoot" -ForegroundColor Cyan
Write-Host "Prompt copied to clipboard." -ForegroundColor Green
Write-Host "Paste it into Copilot." -ForegroundColor Yellow
Write-Host ""