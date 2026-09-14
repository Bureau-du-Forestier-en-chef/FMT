Add-Type -AssemblyName System.Windows.Forms

$ErrorActionPreference = "Stop"

###############################################################################
# Helper functions
###############################################################################

function Show-Message
{
    param(
        [Parameter(Mandatory)]
        [string] $Message,

        [Parameter(Mandatory)]
        [string] $Title,

        [System.Windows.Forms.MessageBoxIcon] $Icon =
            [System.Windows.Forms.MessageBoxIcon]::Information
    )

    [void] [System.Windows.Forms.MessageBox]::Show(
        $Message,
        $Title,
        [System.Windows.Forms.MessageBoxButtons]::OK,
        $Icon
    )
}

function Stop-WithError
{
    param(
        [Parameter(Mandatory)]
        [string] $Message
    )

    Show-Message `
        -Message $Message `
        -Title "Commit Message Generator" `
        -Icon ([System.Windows.Forms.MessageBoxIcon]::Error)

    Write-Error $Message
    exit 1
}

###############################################################################
# Resolve repository root
###############################################################################

try
{
    # Expected script location:
    #
    #   <repository>/tools/commitMessage/Get-CommitMessagePrompt.ps1
    #
    # The repository root is two levels above this script.
    $RepoRoot = Resolve-Path (
        Join-Path $PSScriptRoot "..\.."
    )
}
catch
{
    Stop-WithError (
        "Unable to resolve the repository root.`n`n" +
        $_.Exception.Message
    )
}

Set-Location $RepoRoot

###############################################################################
# Validate the Git repository
###############################################################################

$gitRepositoryCheck = git rev-parse --is-inside-work-tree 2>$null

if ($LASTEXITCODE -ne 0 -or $gitRepositoryCheck -ne "true")
{
    Stop-WithError (
        "The resolved directory is not a Git repository.`n`n" +
        $RepoRoot
    )
}

###############################################################################
# Get staged files
###############################################################################

$files = @(
    git diff --cached --name-status --no-ext-diff
)

if ($LASTEXITCODE -ne 0)
{
    Stop-WithError "Unable to retrieve the staged file list."
}

$filesText = $files -join "`n"

###############################################################################
# Get staged diff
###############################################################################

$diff = @(
    git diff --cached --no-ext-diff --no-color
)

if ($LASTEXITCODE -ne 0)
{
    Stop-WithError "Unable to retrieve the staged diff."
}

$diffText = $diff -join "`n"

###############################################################################
# Stop when there are no staged changes
###############################################################################

if ([System.String]::IsNullOrWhiteSpace($diffText))
{
    Show-Message `
        -Message "No staged changes found." `
        -Title "Commit Message Generator" `
        -Icon ([System.Windows.Forms.MessageBoxIcon]::Information)

    exit 0
}

###############################################################################
# Build the Copilot prompt
###############################################################################

$prompt = @"
Generate one Git commit message that strictly follows the Conventional
Commits 1.0.0 specification:

https://www.conventionalcommits.org/en/v1.0.0/

Required format:

<type>[optional scope][optional !]: <description>

[optional body]

[optional footer(s)]

Rules:

1. Analyze only the staged files and staged diff provided below.

2. Select the most appropriate commit type:
   - feat: introduces a new user-visible feature
   - fix: corrects a defect
   - build: changes the build system or external dependencies
   - refactor: restructures code without changing behavior
   - perf: improves performance
   - docs: changes documentation only
   - test: adds or modifies tests
   - ci: changes continuous integration files or workflows
   - chore: performs maintenance not covered by another type
   - style: changes formatting without affecting behavior

3. Use a lowercase type.

4. Include a concise lowercase scope when it adds useful context.

5. Write the subject in the imperative mood.

6. Do not end the subject with a period.

7. Keep the complete subject line at 72 characters or fewer.

8. Describe the staged changes accurately without inventing changes,
   motivations, issues, or behavior not shown in the diff.

9. Add a body when the subject alone does not adequately explain the change.

10. Separate the subject, body, and footers with blank lines.

11. Keep body lines reasonably short, preferably 72 characters or fewer.

12. Use concise body paragraphs or bullet points to explain:
    - what changed
    - why it changed, only when the reason is evident from the diff
    - relevant implementation details

13. If the staged changes introduce a breaking API or behavior change:
    - add ! immediately before the colon
    - add a footer in this exact form:

      BREAKING CHANGE: <description>

14. Use Git trailer-style footers only when supported by the staged changes,
    for example:

    Refs: #123

15. Output exactly one commit message.

16. Output only the raw commit message.

17. Do not output explanations, analysis, alternatives, headings, Markdown,
    code fences, surrounding quotation marks, or leading and trailing text.

Example output:

feat(python): add packaged type stubs

- generate pybind11 stubs during wheel builds
- package .pyi files and py.typed
- enable IDE completion and static type checking

====================
STAGED FILES
====================

$filesText

====================
STAGED DIFF
====================

$diffText
"@

###############################################################################
# Copy the prompt to the clipboard
###############################################################################

try
{
    Set-Clipboard -Value $prompt
}
catch
{
    Stop-WithError (
        "Unable to copy the prompt to the clipboard.`n`n" +
        $_.Exception.Message
    )
}

###############################################################################
# Notify the user
###############################################################################

Show-Message `
    -Message (
        "A Conventional Commits prompt was copied to the clipboard.`n`n" +
        "Paste it into Copilot to generate the commit message."
    ) `
    -Title "Commit Message Generator" `
    -Icon ([System.Windows.Forms.MessageBoxIcon]::Information)

###############################################################################
# Console summary
###############################################################################

Write-Host ""
Write-Host "Repository:" -ForegroundColor Cyan
Write-Host "  $RepoRoot"

Write-Host ""
Write-Host "Staged files:" -ForegroundColor Cyan

foreach ($file in $files)
{
    Write-Host "  $file"
}

Write-Host ""
Write-Host "Prompt copied to the clipboard." -ForegroundColor Green
Write-Host ""