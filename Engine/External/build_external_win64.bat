@echo off
setlocal
chcp 65001 >nul
cd /d "%~dp0"

set "BASE_DIR=%~dp0"
set "PLATFORM=Windows"
set "ARCH=x64"
set "BIN_DIR=%BASE_DIR%bin\%PLATFORM%\%ARCH%"
set "INC_DIR=%BASE_DIR%inc"

REM 디렉토리 생성
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"

echo ============================================
echo Building External Libraries (Windows x64)
echo ============================================

REM 1. iconv 빌드
set "SRC_DIR=iconv"
echo Building %SRC_DIR%...
cmake -S ./%SRC_DIR% -B ./build/%SRC_DIR%/debug -G "Visual Studio 18 2026" -A x64 -DCMAKE_INSTALL_PREFIX=./build/%SRC_DIR%/install -DBUILD_STATIC=ON -DBUILD_SHARED=OFF -DCMAKE_C_FLAGS_DEBUG="/MDd /Od /Ob0 /Z7 /RTC1 /D_DEBUG"
cmake --build ./build/%SRC_DIR%/debug --config Debug --parallel
cmake --install ./build/%SRC_DIR%/debug --config Debug

cmake -S ./%SRC_DIR% -B ./build/%SRC_DIR%/release -G "Visual Studio 18 2026" -A x64 -DCMAKE_INSTALL_PREFIX=./build/%SRC_DIR%/install -DBUILD_STATIC=ON -DBUILD_SHARED=OFF -DCMAKE_C_FLAGS_RELEASE="/MD /O2 /Ob2 /DNDEBUG /Z7"
cmake --build ./build/%SRC_DIR%/release --config Release --parallel
cmake --install ./build/%SRC_DIR%/release --config Release

xcopy "%BASE_DIR%build\%SRC_DIR%\install\lib\libiconvd.lib" "%BIN_DIR%\" /Y
xcopy "%BASE_DIR%build\%SRC_DIR%\install\lib\libiconv.lib" "%BIN_DIR%\" /Y
xcopy "%BASE_DIR%build\%SRC_DIR%\install\include\*.*" "%INC_DIR%\%SRC_DIR%\" /Y /S

REM 2. mimalloc 빌드
set "SRC_DIR=mimalloc-3.3.1"
echo Building %SRC_DIR%...
cmake -S ./%SRC_DIR% -B ./build/%SRC_DIR%/debug -G "Visual Studio 18 2026" -A x64 -DCMAKE_INSTALL_PREFIX=./build/%SRC_DIR%/install -DMI_BUILD_SHARED=OFF -DMI_BUILD_STATIC=ON -DMI_BUILD_TESTS=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebugDLL -DCMAKE_C_FLAGS_DEBUG="/MDd /Od /Ob0 /Z7 /RTC1 /D_DEBUG"
cmake --build ./build/%SRC_DIR%/debug --config Debug --parallel
cmake --install ./build/%SRC_DIR%/debug --config Debug
cmake -E rename ./build/%SRC_DIR%/install/lib/mimalloc-3.3/mimalloc-debug.lib ./build/%SRC_DIR%/install/lib/mimalloc-3.3/mimalloc_d.lib

cmake -S ./%SRC_DIR% -B ./build/%SRC_DIR%/release -G "Visual Studio 18 2026" -A x64 -DCMAKE_INSTALL_PREFIX=./build/%SRC_DIR%/install -DMI_BUILD_SHARED=OFF -DMI_BUILD_STATIC=ON -DMI_BUILD_TESTS=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL -DCMAKE_C_FLAGS_RELEASE="/MD /O2 /Ob2 /DNDEBUG /Z7"
cmake --build ./build/%SRC_DIR%/release --config Release --parallel
cmake --install ./build/%SRC_DIR%/release --config Release

xcopy "%BASE_DIR%build\%SRC_DIR%\install\lib\mimalloc-3.3\mimalloc_d.lib" "%BIN_DIR%\" /Y
xcopy "%BASE_DIR%build\%SRC_DIR%\install\lib\mimalloc-3.3\mimalloc.lib" "%BIN_DIR%\" /Y
xcopy "%BASE_DIR%build\%SRC_DIR%\install\include\mimalloc-3.3\*.*" "%INC_DIR%\mimalloc\" /Y /S

REM 3. spdlog 등 추가 라이브러리 (필요시 동일한 패턴으로 확장)

REM ============================================
REM 빌드 잔재(obj 등) 및 임시 디렉토리 완벽 삭제
REM ============================================
echo Cleaning up temporary build files...
rmdir "%BASE_DIR%build" /s /q

echo All External libraries built successfully!
endlocal