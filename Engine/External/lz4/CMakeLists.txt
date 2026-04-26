# =============================================================================
# lz4/CMakeLists.txt
# lz4 압축 라이브러리 빌드 정의 (Static Library 전용)
# 이 파일은 lz4/ 소스 폴더 루트에 배치
# =============================================================================
cmake_minimum_required(VERSION 3.15)
project(lz4 VERSION 1.9.4 LANGUAGES C)

# lz4 소스 파일 목록
# xxhash.c 는 lz4frame 내부 체크섬 연산에 사용
set(LZ4_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/lz4.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lz4hc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lz4frame.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/xxhash.c"
)

set(LZ4_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/lz4.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lz4hc.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lz4frame.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lz4frame_static.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/xxhash.h"
)

# -----------------------------------------------------------------------------
# Static Library 타겟
# -----------------------------------------------------------------------------
add_library(lz4 STATIC ${LZ4_SOURCES})

target_include_directories(lz4
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/lz4>
)

# Debug 빌드 시 suffix "d" 추가 → liblz4d.a / lz4d.lib
set_target_properties(lz4 PROPERTIES
    DEBUG_POSTFIX "d"
    C_STANDARD 99
)

# -----------------------------------------------------------------------------
# 플랫폼별 컴파일 옵션
# -----------------------------------------------------------------------------
if(MSVC)
    target_compile_options(lz4 PRIVATE /utf-8 /W3)
    target_compile_definitions(lz4 PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    # Clang / GCC (macOS, iOS, Android)
    target_compile_options(lz4 PRIVATE
        -Wall
        -Wextra
        -Wno-unused-parameter
    )
endif()

# -----------------------------------------------------------------------------
# Install 규칙
# -----------------------------------------------------------------------------
install(TARGETS lz4
    ARCHIVE DESTINATION lib
)
install(FILES ${LZ4_HEADERS}
    DESTINATION include/lz4
)
