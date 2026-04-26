#!/bin/bash
# =============================================================================
# build_external_apple.sh
# lz4 / libuv / ufbx → macOS(Universal) / iOS(arm64) Static Library 빌드
# 각 라이브러리 소스 폴더(lz4/, libuv/, ufbx/)의 CMakeLists.txt 직접 참조
#
# 사용법:
#   ./build_external_apple.sh         ← macOS Universal (arm64 + x86_64)
#   ./build_external_apple.sh ios     ← iOS arm64
# =============================================================================

set -e

PLATFORM="${1:-macos}"

BASE_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_ROOT="$BASE_DIR/build"
INSTALL_ROOT="$BUILD_ROOT/install"
INC_DIR="$BASE_DIR/inc"
CPU_COUNT="$(sysctl -n hw.logicalcpu)"

# 플랫폼별 설정
if [ "$PLATFORM" = "ios" ]; then
    echo "[INFO] 플랫폼: iOS (arm64)"
    ARCHS=("arm64")
    BIN_DIR="$BASE_DIR/bin/ios/universal"       # iOS 출력 경로
    OS_FLAGS=(
        "-DCMAKE_SYSTEM_NAME=iOS"
        "-DCMAKE_OSX_DEPLOYMENT_TARGET=14.0"
        "-DCMAKE_OSX_SYSROOT=iphoneos"
    )
else
    echo "[INFO] 플랫폼: macOS Universal (arm64 + x86_64)"
    ARCHS=("arm64" "x86_64")
    BIN_DIR="$BASE_DIR/bin/macos/universal"     # macOS 출력 경로
    OS_FLAGS=(
        "-DCMAKE_OSX_DEPLOYMENT_TARGET=11.0"
    )
fi

mkdir -p "$BIN_DIR" "$INC_DIR"

# =============================================================================
# 단일 아키텍처 빌드 함수
# $1: 라이브러리명  $2: 아키텍처  $3: 빌드타입
# 소스 경로: $BASE_DIR/$LIB (CMakeLists.txt가 소스 폴더에 직접 배치되어 있음)
# =============================================================================
build_lib()
{
    local LIB="$1" ARCH="$2" BUILD_TYPE="$3"
    local BT_LOWER="$(echo "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')"
    local SRC_DIR="$BASE_DIR/$LIB"                              # 소스 폴더 = CMakeLists.txt 위치
    local BUILD_PATH="$BUILD_ROOT/$LIB/$BT_LOWER/$ARCH"
    local INSTALL_PATH="$INSTALL_ROOT/$LIB/$BT_LOWER/$ARCH"

    echo "  [BUILD] $LIB | $BUILD_TYPE | $ARCH"

    cmake -S "$SRC_DIR" \
          -B "$BUILD_PATH" \
          -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
          -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH" \
          -DCMAKE_OSX_ARCHITECTURES="$ARCH" \
          -DBUILD_SHARED_LIBS=OFF \
          "${OS_FLAGS[@]}"

    cmake --build   "$BUILD_PATH" --config "$BUILD_TYPE" --parallel "$CPU_COUNT"
    cmake --install "$BUILD_PATH" --config "$BUILD_TYPE"
}

# =============================================================================
# Universal Binary 생성 (macOS 전용)
# arm64 + x86_64 .a 를 lipo로 병합
# $1: 라이브러리명  $2: 빌드타입 소문자  $3: .a 파일명  $4: 출력 경로
# =============================================================================
make_universal()
{
    local LIB="$1" BT="$2" A_FILE="$3" OUT="$4"
    local ARM64="$INSTALL_ROOT/$LIB/$BT/arm64/lib/$A_FILE"
    local X86="$INSTALL_ROOT/$LIB/$BT/x86_64/lib/$A_FILE"

    if [ -f "$ARM64" ] && [ -f "$X86" ]; then
        echo "  [LIPO] $A_FILE -> Universal"
        lipo -create "$ARM64" "$X86" -output "$OUT"
    elif [ -f "$ARM64" ]; then
        echo "  [COPY] $A_FILE (arm64 only)"
        cp -f "$ARM64" "$OUT"
    else
        echo "  [WARN] $A_FILE 없음: $ARM64"
    fi
}

# =============================================================================
# 단순 복사 (iOS 전용)
# $1: 라이브러리명  $2: 빌드타입 소문자  $3: .a 파일명  $4: 출력 경로
# =============================================================================
copy_lib()
{
    local LIB="$1" BT="$2" A_FILE="$3" OUT="$4"
    local SRC="$INSTALL_ROOT/$LIB/$BT/${ARCHS[0]}/lib/$A_FILE"

    if [ -f "$SRC" ]; then
        cp -f "$SRC" "$OUT"
        echo "  [COPY] $A_FILE -> $(basename "$OUT")"
    else
        echo "  [WARN] $A_FILE 없음: $SRC"
    fi
}

# =============================================================================
# 헤더 복사 (아키텍처 무관, release 기준)
# $1: 라이브러리명  $2: install 내 헤더 서브경로
# =============================================================================
copy_headers()
{
    local LIB="$1" SUBPATH="$2"
    local SRC="$INSTALL_ROOT/$LIB/release/${ARCHS[0]}/$SUBPATH"

    mkdir -p "$INC_DIR/$LIB"
    if [ -d "$SRC" ]; then
        cp -Rf "$SRC/." "$INC_DIR/$LIB/"
        echo "  [COPY] 헤더 -> inc/$LIB/"
    else
        echo "  [WARN] 헤더 없음: $SRC"
    fi
}

# =============================================================================
# 추가 cmake 옵션을 받는 빌드 함수 (spdlog 등 공식 CMakeLists.txt용)
# $1: 라이브러리명  $2: 아키텍처  $3: 빌드타입  $4~: 추가 cmake 옵션
# =============================================================================
build_lib_extra()
{
    local LIB="$1" ARCH="$2" BUILD_TYPE="$3"
    local BT_LOWER="$(echo "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')"
    local SRC_DIR="$BASE_DIR/$LIB"
    local BUILD_PATH="$BUILD_ROOT/$LIB/$BT_LOWER/$ARCH"
    local INSTALL_PATH="$INSTALL_ROOT/$LIB/$BT_LOWER/$ARCH"
    shift 3

    echo "  [BUILD] $LIB | $BUILD_TYPE | $ARCH"

    cmake -S "$SRC_DIR" \
          -B "$BUILD_PATH" \
          -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
          -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH" \
          -DCMAKE_OSX_ARCHITECTURES="$ARCH" \
          -DBUILD_SHARED_LIBS=OFF \
          "${OS_FLAGS[@]}" \
          "$@"

    cmake --build   "$BUILD_PATH" --config "$BUILD_TYPE" --parallel "$CPU_COUNT"
    cmake --install "$BUILD_PATH" --config "$BUILD_TYPE"
}


# =============================================================================
# lz4
# =============================================================================
echo ""
echo "[INFO] ===== lz4 ====="
for ARCH in "${ARCHS[@]}"; do
    build_lib "lz4" "$ARCH" "Debug"
    build_lib "lz4" "$ARCH" "Release"
done

if [ "$PLATFORM" = "macos" ]; then
    make_universal "lz4" "debug"   "liblz4d.a" "$BIN_DIR/liblz4_d.a"
    make_universal "lz4" "release" "liblz4.a"  "$BIN_DIR/liblz4.a"
else
    copy_lib "lz4" "debug"   "liblz4d.a" "$BIN_DIR/liblz4_d.a"
    copy_lib "lz4" "release" "liblz4.a"  "$BIN_DIR/liblz4.a"
fi
copy_headers "lz4" "include/lz4"

# =============================================================================
# libuv
# =============================================================================
echo ""
echo "[INFO] ===== libuv ====="
for ARCH in "${ARCHS[@]}"; do
    build_lib "libuv" "$ARCH" "Debug"
    build_lib "libuv" "$ARCH" "Release"
done

if [ "$PLATFORM" = "macos" ]; then
    make_universal "libuv" "debug"   "libuvd.a" "$BIN_DIR/libuv_d.a"
    make_universal "libuv" "release" "libuv.a"  "$BIN_DIR/libuv.a"
else
    copy_lib "libuv" "debug"   "libuvd.a" "$BIN_DIR/libuv_d.a"
    copy_lib "libuv" "release" "libuv.a"  "$BIN_DIR/libuv.a"
fi
copy_headers "libuv" "include/libuv"

# =============================================================================
# ufbx
# =============================================================================
echo ""
echo "[INFO] ===== ufbx ====="
for ARCH in "${ARCHS[@]}"; do
    build_lib "ufbx" "$ARCH" "Debug"
    build_lib "ufbx" "$ARCH" "Release"
done

if [ "$PLATFORM" = "macos" ]; then
    make_universal "ufbx" "debug"   "libufbxd.a" "$BIN_DIR/libufbx_d.a"
    make_universal "ufbx" "release" "libufbx.a"  "$BIN_DIR/libufbx.a"
else
    copy_lib "ufbx" "debug"   "libufbxd.a" "$BIN_DIR/libufbx_d.a"
    copy_lib "ufbx" "release" "libufbx.a"  "$BIN_DIR/libufbx.a"
fi
copy_headers "ufbx" "include/ufbx"

# =============================================================================
# spdlog
# 공식 CMakeLists.txt 사용 (spdlog/ 소스 폴더에 이미 존재)
# Static 빌드: SPDLOG_BUILD_SHARED=OFF, fmt는 spdlog 번들 버전 사용
# =============================================================================
echo ""
echo "[INFO] ===== spdlog ====="
for ARCH in "${ARCHS[@]}"; do
    build_lib_extra "spdlog" "$ARCH" "Debug" \
        "-DSPDLOG_BUILD_SHARED=OFF" \
        "-DSPDLOG_FMT_EXTERNAL=OFF" \
        "-DSPDLOG_BUILD_EXAMPLE=OFF" \
        "-DSPDLOG_BUILD_TESTS=OFF" \
        "-DSPDLOG_INSTALL=ON"
    build_lib_extra "spdlog" "$ARCH" "Release" \
        "-DSPDLOG_BUILD_SHARED=OFF" \
        "-DSPDLOG_FMT_EXTERNAL=OFF" \
        "-DSPDLOG_BUILD_EXAMPLE=OFF" \
        "-DSPDLOG_BUILD_TESTS=OFF" \
        "-DSPDLOG_INSTALL=ON"
done

if [ "$PLATFORM" = "macos" ]; then
    make_universal "spdlog" "debug"   "libspdlogd.a" "$BIN_DIR/libspdlog_d.a"
    make_universal "spdlog" "release" "libspdlog.a"  "$BIN_DIR/libspdlog.a"
else
    copy_lib "spdlog" "debug"   "libspdlogd.a" "$BIN_DIR/libspdlog_d.a"
    copy_lib "spdlog" "release" "libspdlog.a"  "$BIN_DIR/libspdlog.a"
fi
# spdlog 헤더는 install 후 include/spdlog/ 에 위치
copy_headers "spdlog" "include/spdlog"


# =============================================================================
# 정리
# =============================================================================
echo ""
echo "[INFO] 빌드 디렉토리 정리 중..."
rm -rf "$BUILD_ROOT"

echo ""
echo "[INFO] 완료!"
echo "  $BIN_DIR : .a 파일"
echo "  $INC_DIR : 헤더 파일"

read -rp "계속하려면 Enter 키를 누르세요..."
