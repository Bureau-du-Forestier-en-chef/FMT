@echo off

powershell -ExecutionPolicy Bypass ^
  -File "%~dp0tools\commitMessage\Get-CommitMessagePrompt.ps1"

pause