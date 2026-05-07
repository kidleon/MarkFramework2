@echo off
setlocal

REM 콘솔 코드페이지를 UTF-8로 (한글 메시지 깨짐 방지)
chcp 65001 >nul

cd /d "%~dp0"

REM 현재 스크립트 위치를 BASE_DIR로 명시적으로 설정
set "BASE_DIR=%~dp0"

set "SRC_DIR=mikktspace"
set "BUILD_DIR=mikktspace-build_win64"

rmdir /S /Q .\build\mikktspace\install
rmdir /S /Q .\build\mikktspace\debug
rmdir /S /Q .\build\mikktspace\release

REM ============================================
REM Debug 빌드
REM ============================================
cmake -S ./mikktspace -B ./build/mikktspace/debug -G "Visual Studio 18 2026" -A x64 ^
    -DCMAKE_INSTALL_PREFIX=./build/mikktspace/install/debug ^
    -DCMAKE_POLICY_DEFAULT_CMP0091=NEW ^
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebugDLL ^
    -DCMAKE_C_FLAGS_DEBUG="/MDd /Od /Ob0 /Z7 /RTC1 /D_DEBUG"

cmake --build ./build/mikktspace/debug --config Debug --parallel
cmake --install ./build/mikktspace/debug --config Debug
cmake -E rename ./build/mikktspace/install/debug/lib/mikktspace.lib ^
                ./build/mikktspace/install/debug/lib/mikktspace_d.lib

REM ============================================
REM Release 빌드 (/Z7로 디버그 정보 obj에 임베드)
REM ============================================
cmake -S ./mikktspace -B ./build/mikktspace/release -G "Visual Studio 18 2026" -A x64 ^
    -DCMAKE_INSTALL_PREFIX=./build/mikktspace/install/release ^
    -DCMAKE_POLICY_DEFAULT_CMP0091=NEW ^
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL ^
    -DCMAKE_C_FLAGS_RELEASE="/MD /O2 /Ob2 /DNDEBUG /Z7"

cmake --build ./build/mikktspace/release --config Release --parallel
cmake --install ./build/mikktspace/release --config Release

REM ============================================
REM 산출물 복사 (lib + 헤더)
REM ============================================
REM .lib 파일들 복사
xcopy "%BASE_DIR%build\mikktspace\install\debug\lib\mikktspace_d.lib" "%BASE_DIR%bin\x64\" /Y
xcopy "%BASE_DIR%build\mikktspace\install\release\lib\mikktspace.lib" "%BASE_DIR%bin\x64\" /Y

REM 헤더 복사
xcopy "%BASE_DIR%build\mikktspace\install\release\include\*.*" "%BASE_DIR%inc\mikktspace\" /Y /S

rmdir "%BASE_DIR%build" /s /q

endlocal