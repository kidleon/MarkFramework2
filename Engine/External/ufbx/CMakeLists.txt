# =============================================================================
# ufbx/CMakeLists.txt
# ufbx FBX 파서 빌드 정의 (Static Library 전용)
# 이 파일은 ufbx/ 소스 폴더 루트에 배치
# ufbx는 ufbx.h + ufbx.c 단일 파일 구조
# =============================================================================
cmake_minimum_required(VERSION 3.15)
project(ufbx VERSION 0.14.0 LANGUAGES C)

# -----------------------------------------------------------------------------
# Static Library 타겟
# ufbx.c 하나로 전체 라이브러리가 구성됨
# -----------------------------------------------------------------------------
add_library(ufbx STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/ufbx.c"
)

target_include_directories(ufbx
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/ufbx>
)

# Debug suffix → libufbxd.a / ufbxd.lib
set_target_properties(ufbx PROPERTIES
    DEBUG_POSTFIX "d"
    C_STANDARD 99
)

# -----------------------------------------------------------------------------
# 플랫폼별 컴파일 옵션
# -----------------------------------------------------------------------------
if(MSVC)
    target_compile_options(ufbx PRIVATE /utf-8 /W3)
    target_compile_definitions(ufbx PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(ufbx PRIVATE
        -Wall
        -Wextra
        -Wno-unused-parameter
    )
endif()

# iOS: Xcode 14+ 에서 Bitcode 비활성화 (기본 비활성화이나 명시적 지정)
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set_target_properties(ufbx PROPERTIES
        XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
    )
endif()

# -----------------------------------------------------------------------------
# Install 규칙
# -----------------------------------------------------------------------------
install(TARGETS ufbx
    ARCHIVE DESTINATION lib
)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/ufbx.h"
    DESTINATION include/ufbx
)
