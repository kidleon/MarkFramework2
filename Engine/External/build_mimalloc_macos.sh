#!/bin/bash
set -e

# 스크립트 위치를 기준으로 BASE_DIR 설정
BASE_DIR="$(cd "$(dirname "$0")" && pwd)"
INSTALL_DIR="$BASE_DIR/build/mimalloc/install"
NCPU=$(sysctl -n hw.logicalcpu)

# 기존 빌드 산출물 초기화
rm -rf "$BASE_DIR/build/mimalloc"

# ============================================
# Debug 빌드
# ============================================
cmake -S "$BASE_DIR/mimalloc-3.3.1" \
      -B "$BASE_DIR/build/mimalloc/debug" \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
      -DMI_BUILD_SHARED=OFF \
      -DMI_BUILD_STATIC=ON \
      -DMI_BUILD_OBJECT=OFF \
      -DMI_BUILD_TESTS=OFF \
      -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
      -DCMAKE_C_FLAGS_DEBUG="-O0 -g -D_DEBUG" \
      -DCMAKE_CXX_FLAGS_DEBUG="-O0 -g -D_DEBUG"

cmake --build "$BASE_DIR/build/mimalloc/debug"   --config Debug   --parallel "$NCPU"
cmake --install "$BASE_DIR/build/mimalloc/debug" --config Debug

# 디버그 lib 이름 통일 (mimalloc-debug.a → mimalloc_d.a)
mv "$INSTALL_DIR/lib/mimalloc-3.3/libmimalloc-debug.a" \
   "$INSTALL_DIR/lib/mimalloc-3.3/libmimalloc_d.a"

# ============================================
# Release 빌드 (/Z7 대응: -g 로 obj에 디버그 정보 포함)
# ============================================
cmake -S "$BASE_DIR/mimalloc-3.3.1" \
      -B "$BASE_DIR/build/mimalloc/release" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
      -DMI_BUILD_SHARED=OFF \
      -DMI_BUILD_STATIC=ON \
      -DMI_BUILD_OBJECT=OFF \
      -DMI_BUILD_TESTS=OFF \
      -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
      -DCMAKE_C_FLAGS_RELEASE="-O2 -DNDEBUG -g" \
      -DCMAKE_CXX_FLAGS_RELEASE="-O2 -DNDEBUG -g"

cmake --build "$BASE_DIR/build/mimalloc/release"   --config Release --parallel "$NCPU"
cmake --install "$BASE_DIR/build/mimalloc/release" --config Release

# ============================================
# 산출물 복사 (lib + 헤더)
# ============================================
mkdir -p "$BASE_DIR/bin/macos/universal"
mkdir -p "$BASE_DIR/inc/mimalloc"

cp "$INSTALL_DIR/lib/mimalloc-3.3/libmimalloc_d.a" "$BASE_DIR/bin/macos/universal/"
cp "$INSTALL_DIR/lib/mimalloc-3.3/libmimalloc.a"   "$BASE_DIR/bin/macos/universal/"

# universal binary 확인 (arm64 + x86_64 슬라이스 포함 여부)
lipo -info "$BASE_DIR/bin/macos/universal/libmimalloc_d.a"
lipo -info "$BASE_DIR/bin/macos/universal/libmimalloc.a"

cp -R "$INSTALL_DIR/include/mimalloc-3.3/." "$BASE_DIR/inc/mimalloc/"

# 빌드 임시 디렉토리 정리
rm -rf "$BASE_DIR/build"

echo "Done. libs → bin/macos/universal/  headers → inc/mimalloc/"