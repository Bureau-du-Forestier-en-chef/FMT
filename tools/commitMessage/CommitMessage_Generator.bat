@echo off

powershell -ExecutionPolicy Bypass ^
  -File "%~dp0Get-CommitMessagePrompt.ps1"

pause