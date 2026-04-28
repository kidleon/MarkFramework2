@echo off
setlocal

REM 콘솔 코드페이지를 UTF-8로 (한글 메시지 깨짐 방지)
chcp 65001 >nul

cd /d "%~dp0"

REM BASE_DIR 변수가 비어있어서 xcopy가 작동 안 했을 가능성도 있음
REM 현재 스크립트 위치를 BASE_DIR로 명시적으로 설정
set "BASE_DIR=%~dp0"

set "SRC_DIR=mimalloc-3.3.1"
set "BUILD_DIR=mimalloc-3.3.1-build_win64"

rmdir /S /Q .\build\mimalloc\install
rmdir /S /Q .\build\mimalloc\debug
rmdir /S /Q .\build\mimalloc\release

REM ============================================
REM Debug 빌드
REM ============================================
REM Debug는 /Zi가 기본이라 Compiler PDB가 자동 생성됨
cmake -S ./mimalloc-3.3.1 -B ./build/mimalloc/debug -G "Visual Studio 18 2026" -A x64 ^
    -DCMAKE_INSTALL_PREFIX=./build/mimalloc/install ^
    -DMI_BUILD_SHARED=OFF -DMI_BUILD_STATIC=ON -DMI_BUILD_OBJECT=OFF ^
    -DMI_BUILD_TESTS=OFF -DMI_WIN_USE_FIXED_TLS=ON ^
    -DCMAKE_POLICY_DEFAULT_CMP0091=NEW ^
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebugDLL ^
    -DCMAKE_C_FLAGS_DEBUG="/MDd /Od /Ob0 /Z7 /RTC1 /D_DEBUG" ^
    -DCMAKE_CXX_FLAGS_DEBUG="/MDd /Od /Ob0 /Z7 /RTC1 /D_DEBUG"

cmake --build ./build/mimalloc/debug --config Debug --parallel
cmake --install ./build/mimalloc/debug --config Debug
cmake -E rename ./build/mimalloc/install/lib/mimalloc-3.3/mimalloc-debug.lib ^
                ./build/mimalloc/install/lib/mimalloc-3.3/mimalloc_d.lib

REM ============================================
REM Release 빌드 - PDB 생성 플래그 명시적으로 추가
REM ============================================
REM CMAKE_C_FLAGS_RELEASE에 /Zi 추가: 컴파일 시 디버그 정보 생성
REM (static lib이라 링커 PDB는 어차피 안 나옴, Compiler PDB만 노림)
REM /Zi: 별도 PDB 파일에 디버그 정보 저장 (PDB 통합 가능)
REM /Z7: obj 파일 안에 직접 디버그 정보 임베드 (PDB 파일 안 생김, 단일 obj로 자족)
REM      → static lib에는 /Z7이 더 편할 수도 있음 (PDB 따로 안 챙겨도 됨)
cmake -S ./mimalloc-3.3.1 -B ./build/mimalloc/release -G "Visual Studio 18 2026" -A x64 ^
    -DCMAKE_INSTALL_PREFIX=./build/mimalloc/install ^
    -DMI_BUILD_SHARED=OFF -DMI_BUILD_STATIC=ON -DMI_BUILD_OBJECT=OFF ^
    -DMI_BUILD_TESTS=OFF -DMI_WIN_USE_FIXED_TLS=ON ^
    -DCMAKE_POLICY_DEFAULT_CMP0091=NEW ^
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL ^
    -DCMAKE_C_FLAGS_RELEASE="/MD /O2 /Ob2 /DNDEBUG /Z7" ^
    -DCMAKE_CXX_FLAGS_RELEASE="/MD /O2 /Ob2 /DNDEBUG /Z7"

cmake --build ./build/mimalloc/release --config Release --parallel
cmake --install ./build/mimalloc/release --config Release

REM ============================================
REM 산출물 복사 (lib + pdb + 헤더)
REM ============================================
REM .lib 파일들 복사
xcopy "%BASE_DIR%build\mimalloc\install\lib\mimalloc-3.3\mimalloc_d.lib" "%BASE_DIR%bin\x64\" /Y
xcopy "%BASE_DIR%build\mimalloc\install\lib\mimalloc-3.3\mimalloc.lib"   "%BASE_DIR%bin\x64\" /Y

REM 헤더 복사
xcopy "%BASE_DIR%build\mimalloc\install\include\mimalloc-3.3\*.*" "%BASE_DIR%inc\mimalloc\" /Y /S

rmdir "%BASE_DIR%build" /s /q

endlocal
