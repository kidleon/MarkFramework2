@echo off
setlocal

REM 콘솔 코드페이지를 UTF-8 로 (한글 메시지 깨짐 방지)
chcp 65001 >nul

cd /d "%~dp0"

set "SRC_DIR=mimalloc-3.3.1"
set "BUILD_DIR=mimalloc-3.3.1-build_win64"

rmdir /S /Q .\build\mimalloc\install
rmdir /S /Q .\build\mimalloc\debug
rmdir /S /Q .\build\mimalloc\release

REM  Build Debug
cmake -S ./mimalloc-3.3.1 -B ./build/mimalloc/debug -G "Visual Studio 18 2026" -A x64 -DCMAKE_INSTALL_PREFIX=./build/mimalloc/install -DMI_BUILD_SHARED=OFF -DMI_BUILD_STATIC=ON -DMI_BUILD_OBJECT=OFF -DMI_BUILD_TESTS=OFF -DMI_WIN_USE_FIXED_TLS=ON 
cmake --build ./build/mimalloc/debug --config Debug --parallel 
cmake --install ./build/mimalloc/debug --config Debug
cmake -E rename ./build/mimalloc/install/lib/mimalloc-3.3/mimalloc-debug.lib ./build/mimalloc/install/lib/mimalloc-3.3/mimalloc_d.lib


REM Build Release
cmake -S ./mimalloc-3.3.1 -B ./build/mimalloc/release -G "Visual Studio 18 2026" -A x64 -DCMAKE_INSTALL_PREFIX=./build/mimalloc/install -DMI_BUILD_SHARED=OFF -DMI_BUILD_STATIC=ON -DMI_BUILD_OBJECT=OFF -DMI_BUILD_TESTS=OFF -DMI_WIN_USE_FIXED_TLS=ON 
cmake --build ./build/mimalloc/release --config Release --parallel 
cmake --install ./build/mimalloc/release --config Release

xcopy "%BASE_DIR%build\mimalloc\install\lib\mimalloc-3.3\mimalloc_d.lib" "%BASE_DIR%bin\x64\" /Y
xcopy "%BASE_DIR%build\mimalloc\install\lib\mimalloc-3.3\mimalloc.lib" "%BASE_DIR%bin\x64\" /Y
xcopy "%BASE_DIR%build\mimalloc\install\include\mimalloc-3.3\*.*" "%BASE_DIR%inc\mimalloc\" /Y /S

rmdir "%BASE_DIR%build" /s /q

endlocal

