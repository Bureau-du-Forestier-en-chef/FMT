<#
.SYNOPSIS
Renames the test executables in the private copy of BFECtests.csv of a worktree, after the renaming of
the C++ test executables under issue #350.

.DESCRIPTION
The private tests are the rows of BFECtests.csv, a file that each worktree keeps for itself and that git
ignores, so the renaming of the test executables could not reach it. This script brings one copy up to
date with Tests/Support/RenamedTests.csv, the former;new table of the worktree:

- a copy still in Examples/C++/tests, its folder before phase 6 of the test reorganization, is first
  moved to Tests/System/Cpp, unless that folder already has one;
- the first column of each row, its test executable, takes its new name;
- an output folder named after a renamed executable, ../../tests/<former name>, takes its new name in
  the arguments, as in basetests.csv.

The file is written only when something changes, after a copy of it is kept beside it, as
BFECtests.csv.<date>-<time>.bak, which git ignores. A second run therefore changes nothing. Only counts
and paths are written to the console, never the content of a row: the rows name private data.

The configuration reads the former names until the copy is migrated, with a warning that counts them.

.EXAMPLE
Update-PrivateTests.ps1
Migrates the copy of the worktree that holds this script.

.EXAMPLE
Update-PrivateTests.ps1 -Worktree C:\Users\me\Documents\FMT\dev
Migrates the copy of another worktree. That worktree must have the renamed executables already: its
Tests/Support/RenamedTests.csv is the table that the script applies.
#>
param(
    # Worktree whose copy of BFECtests.csv is migrated. Default: the one that holds this script.
    [string] $Worktree = ""
)

$ErrorActionPreference = "Stop"

# Latin-1 maps each byte to one character and back: the file is rewritten byte for byte, whatever its
# encoding, apart from the names that change.
$bytesAsText = [System.Text.Encoding]::GetEncoding(28591)

if ($Worktree -eq "")
{
    $Worktree = Join-Path $PSScriptRoot "..\.."
}
$Worktree = (Resolve-Path -LiteralPath $Worktree).Path

$table = Join-Path $Worktree "Tests\Support\RenamedTests.csv"
if (-not (Test-Path -LiteralPath $table -PathType Leaf))
{
    Write-Host "$table was not found: this worktree does not have the renamed test executables yet, so its copy of BFECtests.csv must keep the former names."
    exit 1
}

# former name -> new name, case-sensitive like the CMake targets.
$renamed = New-Object "System.Collections.Generic.Dictionary[string,string]" ([System.StringComparer]::Ordinal)
foreach ($line in [System.IO.File]::ReadAllLines($table, $bytesAsText))
{
    $fields = $line.Split(";")
    if ($fields.Count -ge 2 -and $fields[0] -ne "" -and $fields[0] -ne "former")
    {
        $renamed[$fields[0]] = $fields[1]
    }
}
$newNames = New-Object "System.Collections.Generic.HashSet[string]" ([System.StringComparer]::Ordinal)
foreach ($name in $renamed.Values)
{
    [void] $newNames.Add($name)
}

$current = Join-Path $Worktree "Tests\System\Cpp\BFECtests.csv"
$former = Join-Path $Worktree "Examples\C++\tests\BFECtests.csv"
$moved = $false
if (-not (Test-Path -LiteralPath $current -PathType Leaf))
{
    if (-not (Test-Path -LiteralPath $former -PathType Leaf))
    {
        Write-Host "No private copy of BFECtests.csv in $Worktree : nothing to migrate."
        exit 0
    }
    Move-Item -LiteralPath $former -Destination $current
    $moved = $true
    Write-Host "Moved $former to $current"
}
elseif (Test-Path -LiteralPath $former -PathType Leaf)
{
    Write-Host "Warning: $former is still there. The configuration no longer reads it, since $current exists; compare the two, then delete it."
}

$original = $bytesAsText.GetString([System.IO.File]::ReadAllBytes($current))

# An output folder of a renamed executable, as an argument: ../../tests/<former name>, followed by the
# end of the folder name.
$folderNames = ($renamed.Keys | Sort-Object Length -Descending | ForEach-Object { [regex]::Escape($_) }) -join "|"
$folderPattern = [regex] ('\.\./\.\./tests/(' + $folderNames + ')(?=[/;|]|$)')

$renamedRows = 0
$renamedFolderRows = 0
$unknownRows = 0
$builder = New-Object System.Text.StringBuilder
# Each piece is a line with its own end of line, so that the line endings are kept as they are.
foreach ($piece in [regex]::Split($original, '(?<=\n)'))
{
    if ($piece -eq "")
    {
        continue
    }
    $content = $piece.TrimEnd("`r", "`n")
    $ending = $piece.Substring($content.Length)
    if ($content -ne "")
    {
        $separator = $content.IndexOf(";")
        if ($separator -lt 0)
        {
            $name = $content
            $arguments = ""
        }
        else
        {
            $name = $content.Substring(0, $separator)
            $arguments = $content.Substring($separator)
        }
        if ($renamed.ContainsKey($name))
        {
            $name = $renamed[$name]
            $renamedRows++
        }
        elseif ($name -ne "TEST" -and -not $newNames.Contains($name))
        {
            $unknownRows++
        }
        $newArguments = $folderPattern.Replace($arguments, { param($match) "../../tests/" + $renamed[$match.Groups[1].Value] })
        if ($newArguments -cne $arguments)
        {
            $renamedFolderRows++
        }
        $content = $name + $newArguments
    }
    [void] $builder.Append($content).Append($ending)
}
$migrated = $builder.ToString()

if ($unknownRows -gt 0)
{
    Write-Host "Warning: $unknownRows rows name no test executable of the table; the configuration reports them as 'no target named'."
}
if ($migrated -ceq $original)
{
    if ($moved)
    {
        Write-Host "$current already uses the new names."
    }
    else
    {
        Write-Host "Nothing to change: $current already uses the new names."
    }
    exit 0
}

$backup = "$current." + (Get-Date -Format "yyyyMMdd-HHmmss") + ".bak"
[System.IO.File]::WriteAllBytes($backup, $bytesAsText.GetBytes($original))
[System.IO.File]::WriteAllBytes($current, $bytesAsText.GetBytes($migrated))
Write-Host "$current : $renamedRows rows renamed, $renamedFolderRows rows with a renamed output folder."
Write-Host "Backup of the former content: $backup"
Write-Host "Reconfigure CMake so that ctest registers the rows under their new names."
exit 0
