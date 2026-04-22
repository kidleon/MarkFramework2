@echo off
setlocal
SET version=%~1%
if "%~1"=="" SET version="vs2026"

call premake5.exe %version%

msbuild build/MarkFrameworkExternal.slnx /t:Build /p:Configuration=Debug /p:Platform=x64 /m /nologo /v:minimal
msbuild build/MarkFrameworkExternal.slnx /t:Build /p:Configuration=Release /p:Platform=x64 /m /nologo /v:minimal

build_spdlog.bat

PAUSE