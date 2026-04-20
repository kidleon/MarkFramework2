@echo off
SET version=%~1%
if "%~1"=="" SET version="vs2026"

call premake5.exe %version%

PAUSE