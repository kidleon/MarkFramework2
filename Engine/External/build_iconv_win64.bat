@echo off
setlocal

REM 콘솔 코드페이지를 UTF-8로 (한글 메시지 깨짐 방지)
chcp 65001 >nul

REM 스크립트 자신의 디렉토리로 이동
cd /d "%~dp0"

REM 산출물 복사 시 사용할 기준 경로
set "BASE_DIR=%~dp0"

set "SRC_DIR=iconv"

REM ============================================
REM 기존 빌드 산출물 정리
REM ============================================
rmdir /S /Q .\build\iconv\install
rmdir /S /Q .\build\iconv\debug
rmdir /S /Q .\build\iconv\release

REM ============================================
REM Debug 빌드 (/MDd + /Z7)
REM ============================================
REM /Z7 : 디버그 정보를 obj에 임베드 (별도 PDB 파일 미생성)
REM /MDd: Debug Dynamic CRT (게임 프로젝트와 일치시키기 위함)
REM BUILD_STATIC=ON / BUILD_SHARED=OFF: 정적 라이브러리만 생성
REM BUILD_EXECUTABLE=OFF / BUILD_TEST=OFF: 불필요한 실행파일/테스트 제외
cmake -S ./%SRC_DIR% -B ./build/iconv/debug -G "Visual Studio 18 2026" -A x64 ^
    -DCMAKE_INSTALL_PREFIX=./build/iconv/install ^
    -DBUILD_STATIC=ON -DBUILD_SHARED=OFF ^
    -DBUILD_EXECUTABLE=OFF -DBUILD_TEST=OFF ^
    -DCMAKE_C_FLAGS_DEBUG="/MDd /Od /Ob0 /Z7 /RTC1 /D_DEBUG" ^
    -DCMAKE_CXX_FLAGS_DEBUG="/MDd /Od /Ob0 /Z7 /RTC1 /D_DEBUG"

cmake --build ./build/iconv/debug --config Debug --parallel
cmake --install ./build/iconv/debug --config Debug

REM Debug lib 이름을 iconv_d.lib로 통일 (mimalloc과 동일한 컨벤션)
REM win-iconv는 기본적으로 Debug에서도 iconv.lib로 떨어지므로 rename 필요
REM 만약 출력이 iconvd.lib나 다른 이름이면 그에 맞게 수정

REM ============================================
REM Release 빌드 (/MD + /Z7)
REM ============================================
cmake -S ./%SRC_DIR% -B ./build/iconv/release -G "Visual Studio 18 2026" -A x64 ^
    -DCMAKE_INSTALL_PREFIX=./build/iconv/install ^
    -DBUILD_STATIC=ON -DBUILD_SHARED=OFF ^
    -DBUILD_EXECUTABLE=OFF -DBUILD_TEST=OFF ^
    -DCMAKE_C_FLAGS_RELEASE="/MD /O2 /Ob2 /DNDEBUG /Z7" ^
    -DCMAKE_CXX_FLAGS_RELEASE="/MD /O2 /Ob2 /DNDEBUG /Z7"

cmake --build ./build/iconv/release --config Release --parallel
cmake --install ./build/iconv/release --config Release

REM ============================================
REM 산출물 복사
REM ============================================
REM .lib 복사 - Debug/Release 둘 다 install 폴더의 lib 디렉토리에서 가져옴
xcopy "%BASE_DIR%build\iconv\install\lib\libiconvd.lib" "%BASE_DIR%bin\x64\" /Y
xcopy "%BASE_DIR%build\iconv\install\lib\libiconv.lib"   "%BASE_DIR%bin\x64\" /Y

REM 헤더 복사 - iconv는 보통 iconv.h 한 개만 설치됨
xcopy "%BASE_DIR%build\iconv\install\include\*.*" "%BASE_DIR%inc\iconv\" /Y /S

REM 빌드 중간 산출물 정리
rmdir "%BASE_DIR%build" /s /q

endlocal