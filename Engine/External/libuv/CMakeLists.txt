# =============================================================================
# libuv/CMakeLists.txt
# libuv 비동기 I/O 라이브러리 빌드 정의 (Static Library 전용)
# 이 파일은 libuv/ 소스 폴더 루트에 배치
# 플랫폼(WIN32 / APPLE / ANDROID)에 따라 소스 자동 선택
# =============================================================================
cmake_minimum_required(VERSION 3.15)
project(libuv VERSION 1.48.0 LANGUAGES C)

# -----------------------------------------------------------------------------
# 공통 소스 (모든 플랫폼)
# -----------------------------------------------------------------------------
set(LIBUV_COMMON_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/fs-poll.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/idna.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/inet.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/random.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/strscpy.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/strtok.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/threadpool.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/timer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/uv-common.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/uv-data-getter-setters.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.c"
)

# -----------------------------------------------------------------------------
# Windows 전용 소스
# -----------------------------------------------------------------------------
set(LIBUV_WINDOWS_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/async.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/core.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/detect-wakeup.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/dl.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/error.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/fs.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/fs-event.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/getaddrinfo.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/getnameinfo.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/handle.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/loop-watcher.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/pipe.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/process.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/process-stdio.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/signal.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/snprintf.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/stream.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/tcp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/thread.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/tty.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/udp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/util.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/winapi.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/win/winsock.c"
)

# -----------------------------------------------------------------------------
# Unix 공통 소스 (macOS / iOS / Android 공유)
# -----------------------------------------------------------------------------
set(LIBUV_UNIX_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/async.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/core.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/dl.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/fs.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/getaddrinfo.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/getnameinfo.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/loop.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/loop-watcher.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/pipe.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/poll.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/process.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/random-devurandom.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/signal.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/stream.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/tcp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/thread.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/tty.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/udp.c"
)

# -----------------------------------------------------------------------------
# macOS / iOS 전용 소스 (kqueue 이벤트 루프)
# -----------------------------------------------------------------------------
set(LIBUV_DARWIN_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/darwin.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/darwin-proctitle.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/fsevents.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/kqueue.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/random-getentropy.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/proctitle.c"
)

# -----------------------------------------------------------------------------
# Android 전용 소스 (epoll 이벤트 루프)
# -----------------------------------------------------------------------------
set(LIBUV_ANDROID_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/linux.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/epoll.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/proctitle.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/random-getrandom.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/random-sysctl-linux.c"
)

# -----------------------------------------------------------------------------
# 플랫폼별 소스 선택
# -----------------------------------------------------------------------------
if(WIN32)
    set(LIBUV_PLATFORM_SOURCES ${LIBUV_WINDOWS_SOURCES})
elseif(ANDROID)
    set(LIBUV_PLATFORM_SOURCES ${LIBUV_UNIX_SOURCES} ${LIBUV_ANDROID_SOURCES})
elseif(APPLE)
    set(LIBUV_PLATFORM_SOURCES ${LIBUV_UNIX_SOURCES} ${LIBUV_DARWIN_SOURCES})
endif()

# -----------------------------------------------------------------------------
# Static Library 타겟
# -----------------------------------------------------------------------------

# 타겟명을 "uv"로 지정 → 출력 파일: libuv.a / libuvd.a (lib + uv + [d])
# "libuv"로 지정하면 libllibuv.a 가 되므로 주의
add_library(uv STATIC
    ${LIBUV_COMMON_SOURCES}
    ${LIBUV_PLATFORM_SOURCES}
)

target_include_directories(uv
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/libuv>
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"   # 내부 헤더 참조용
)

# Debug suffix → libuvd.a / uvd.lib
set_target_properties(uv PROPERTIES
    DEBUG_POSTFIX "d"
    C_STANDARD 99
    OUTPUT_NAME "uv"                        # 명시적으로 출력 파일명 고정
)

# -----------------------------------------------------------------------------
# 플랫폼별 컴파일 정의 및 링크 라이브러리
# -----------------------------------------------------------------------------
if(WIN32)
    target_compile_options(uv PRIVATE /utf-8 /W3)
    target_compile_definitions(uv PRIVATE
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
        _WIN32_WINNT=0x0600     # Windows Vista 이상
        _GNU_SOURCE
    )
    target_link_libraries(uv PRIVATE
        ws2_32      # Winsock2
        psapi       # Process Status API
        iphlpapi    # IP Helper API
        userenv     # 사용자 환경
        advapi32    # 고급 Windows API
    )
elseif(ANDROID)
    target_compile_definitions(uv PRIVATE _GNU_SOURCE ANDROID)
    target_compile_options(uv PRIVATE -Wall -Wextra -Wno-unused-parameter)
    target_link_libraries(uv PRIVATE pthread dl)
elseif(APPLE)
    target_compile_definitions(uv PRIVATE _GNU_SOURCE)
    target_compile_options(uv PRIVATE -Wall -Wextra -Wno-unused-parameter)
    target_link_libraries(uv PRIVATE
        "-framework CoreFoundation"
        "-framework CoreServices"
    )
endif()

# -----------------------------------------------------------------------------
# Install 규칙
# -----------------------------------------------------------------------------
install(TARGETS uv
    ARCHIVE DESTINATION lib
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION include/libuv
    FILES_MATCHING PATTERN "*.h"
)