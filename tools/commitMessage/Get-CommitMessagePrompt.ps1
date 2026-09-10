Add-Type -AssemblyName System.Windows.Forms

# Repository root (FMT)
$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")

Set-Location $RepoRoot

# Get staged files
$files = git diff --cached --name-status

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

$prompt = @"
Generate a Conventional Commit message following:
https://www.conventionalcommits.org/en/v1.0.0/

Requirements:
- Determine the most appropriate type (feat, fix, build, refactor, docs, test, chore, ci, perf).
- Use a scope when appropriate.
- Subject line should be concise (less than 72 characters).
- Include a body when useful.
- Base the message ONLY on the staged changes shown below.
- Output ONLY the commit message.
- Do NOT output explanations.
- Do NOT output markdown.
- Do NOT output code fences.
- Enclose the ENTIRE output in double quotes.

Example:

"feat(python): add packaged type stubs

- generate pybind11 stubs during wheel build
- package .pyi files and py.typed
- enable IDE autocomplete and static type checking"

====================
STAGED FILES
====================

$files

====================
STAGED DIFF
====================

$diff

"@

Set-Clipboard -Value $prompt

[System.Windows.Forms.MessageBox]::Show(
    "Prompt copied to clipboard.`nPaste it into Copilot.",
    "Commit Message Generator"
)

Write-Host ""
Write-Host "Repository: $RepoRoot" -ForegroundColor Cyan
Write-Host "Prompt copied to clipboard." -ForegroundColor Green
Write-Host ""