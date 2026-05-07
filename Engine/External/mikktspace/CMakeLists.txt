cmake_minimum_required(VERSION 3.15)
project(mikktspace C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 정적 라이브러리 빌드
add_library(mikktspace STATIC
    mikktspace.c
)

target_include_directories(mikktspace PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# PIC 활성화 (Android/iOS/Linux 공유 라이브러리 호환)
set_target_properties(mikktspace PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# 컴파일러별 경고 옵션
if(MSVC)
    target_compile_options(mikktspace PRIVATE /W3)
else()
    target_compile_options(mikktspace PRIVATE -Wall -Wextra)
endif()

# install 규칙
install(TARGETS mikktspace
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES mikktspace.h
    DESTINATION include
)