-- =============================================================================
-- MarkFrameworkExternal - premake5.lua
-- 지원 플랫폼: Windows (Visual Studio), macOS/iOS (Xcode), Android (VSCode/Makefile)
-- =============================================================================

workspace "MarkFrameworkExternal"
do
	-- ---------------------------------------------------------------------------
	-- 공통 설정
	-- ---------------------------------------------------------------------------
	language "C"
	configurations { "Debug", "Release" }
	characterset ("MBCS")
	location "build"

	-- ---------------------------------------------------------------------------
	-- 플랫폼별 아키텍처 설정
	-- ---------------------------------------------------------------------------

	-- Visual Studio: Windows x64
	filter { "action:vs*" }
		platforms { "x64" }
		architecture "x86_64"

	-- Xcode: macOS Universal Binary (Apple Silicon + Intel)
	filter { "action:xcode*", "system:macosx" }
		platforms { "Universal" }
		architecture "universal"
		buildoptions { "-arch arm64", "-arch x86_64" }

	-- Xcode: iOS ARM64 전용
	filter { "action:xcode*", "system:ios" }
		platforms { "ARM64" }
		architecture "arm64"

	-- VSCode / gmake2: Android (NDK Clang 기반)
	-- ANDROID_NDK 환경변수가 설정되어 있어야 함
	filter { "action:gmake2" }
		platforms { "ARM64" }
		architecture "arm64"
		toolset "clang"

	-- ---------------------------------------------------------------------------
	-- 빌드 구성 (Debug / Release)
	-- ---------------------------------------------------------------------------
	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG", "RELEASE" }
		optimize "On"
		symbols "On"

	-- ---------------------------------------------------------------------------
	-- 출력 디렉토리 설정
	-- ---------------------------------------------------------------------------
	filter {}  -- 이전 필터 초기화

	objoutdir  = "%{wks.location}/Intermediate/"
	outputdir  = "%{wks.location}/../bin/%{cfg.system}_%{cfg.platform}"

	-- ==========================================================================
	-- 프로젝트: lz4 (압축 라이브러리)
	-- ==========================================================================
	project "lz4"
	do
		kind "StaticLib"

		files
		{
			"lz4/**.h",
			"lz4/**.c"
		}

		includedirs { "lz4" }

		targetdir (outputdir)
		objdir    (objoutdir)

		-- Debug 빌드 suffix
		filter "configurations:Debug"
			targetname "lz4_d"

		filter "configurations:Release"
			targetname "lz4"

		-- -----------------------------------------------------------------------
		-- Visual Studio (Windows)
		-- -----------------------------------------------------------------------
		filter { "action:vs*" }
			system       "Windows"
			buildoptions { "/utf-8" }
			defines      { "_CRT_SECURE_NO_WARNINGS" }

			-- 헤더 파일을 공용 inc 디렉토리로 복사
			postbuildcommands
			{
				"echo f | xcopy /I /Y $(ProjectDir)..\\lz4\\*.h $(ProjectDir)..\\inc\\lz4\\*.h"
			}

		-- -----------------------------------------------------------------------
		-- Xcode (macOS)
		-- -----------------------------------------------------------------------
		filter { "action:xcode*", "system:macosx" }
			system       "macosx"
			-- macOS 최소 버전 지정 (필요 시 조정)
			buildoptions { "-mmacosx-version-min=11.0" }

			-- 헤더 파일을 inc 디렉토리로 복사 (쉘 스크립트)
			postbuildcommands
			{
				"cp -R %{wks.location}../lz4/*.h %{wks.location}../inc/lz4/"
			}

		-- -----------------------------------------------------------------------
		-- Xcode (iOS)
		-- -----------------------------------------------------------------------
		filter { "action:xcode*", "system:ios" }
			system       "ios"
			buildoptions { "-mios-version-min=14.0" }

			postbuildcommands
			{
				"cp -R %{wks.location}../lz4/*.h %{wks.location}../inc/lz4/"
			}

		-- -----------------------------------------------------------------------
		-- VSCode / gmake2 (Android NDK)
		-- -----------------------------------------------------------------------
		filter { "action:gmake2" }
			system       "android"
			-- NDK 경로는 환경변수 ANDROID_NDK 참조
			-- 예: export ANDROID_NDK=/path/to/ndk
			buildoptions
			{
				"--sysroot=$(ANDROID_NDK)/toolchains/llvm/prebuilt/linux-x86_64/sysroot",
				"-target aarch64-linux-android21"
			}

			postbuildcommands
			{
				"cp -R %{wks.location}../lz4/*.h %{wks.location}../inc/lz4/"
			}

		filter {}
	end

	-- ==========================================================================
	-- 프로젝트: libuv (비동기 I/O 라이브러리)
	-- ==========================================================================
	project "libuv"
	do
		kind "StaticLib"

		includedirs
		{
			"libuv/include",
			"libuv/src"
		}

		targetdir (outputdir)
		objdir    (objoutdir)

		filter "configurations:Debug"
			targetname "libuv_d"

		filter "configurations:Release"
			targetname "libuv"

		-- -----------------------------------------------------------------------
		-- Visual Studio (Windows)
		-- libuv는 플랫폼별 소스가 완전히 분리되어 있어 명시적으로 지정
		-- -----------------------------------------------------------------------
		filter { "action:vs*" }
			system       "Windows"
			buildoptions { "/utf-8" }
			defines
			{
				"_CRT_SECURE_NO_WARNINGS",
				"WIN32_LEAN_AND_MEAN",
				"_WIN32_WINNT=0x0600"  -- Windows Vista 이상
			}

			files
			{
				"libuv/include/**.h",
				"libuv/src/*.h",
				"libuv/src/*.c",
				"libuv/src/win/**.h",   -- Windows 전용 소스
				"libuv/src/win/**.c"
			}

			-- Unix 소스 제외
			removefiles { "libuv/src/unix/**" }

			links
			{
				"ws2_32",   -- Winsock2
				"psapi",    -- Process Status API
				"iphlpapi", -- IP Helper API
				"userenv",  -- 사용자 환경
				"advapi32"  -- 고급 Windows API
			}

			postbuildcommands
			{
				"echo f | xcopy /I /Y /S $(ProjectDir)..\\libuv\\include\\*.* $(ProjectDir)..\\inc\\libuv\\*.*"
			}

		-- -----------------------------------------------------------------------
		-- Xcode (macOS)
		-- -----------------------------------------------------------------------
		filter { "action:xcode*", "system:macosx" }
			system       "macosx"
			buildoptions { "-mmacosx-version-min=11.0" }

			files
			{
				"libuv/include/**.h",
				"libuv/src/*.h",
				"libuv/src/*.c",
				"libuv/src/unix/**.h",  -- Unix 공통 소스 포함
				"libuv/src/unix/**.c"
			}

			-- Windows 전용 소스 제외
			removefiles { "libuv/src/win/**" }

			-- macOS 전용 시스템 프레임워크 링크
			links
			{
				"CoreFoundation.framework",
				"CoreServices.framework"
			}

			postbuildcommands
			{
				"cp -R %{wks.location}../libuv/include/ %{wks.location}../inc/libuv/"
			}

		-- -----------------------------------------------------------------------
		-- Xcode (iOS)
		-- iOS는 kqueue 기반, CoreFoundation 링크
		-- -----------------------------------------------------------------------
		filter { "action:xcode*", "system:ios" }
			system       "ios"
			buildoptions { "-mios-version-min=14.0" }

			files
			{
				"libuv/include/**.h",
				"libuv/src/*.h",
				"libuv/src/*.c",
				"libuv/src/unix/**.h",
				"libuv/src/unix/**.c"
			}

			removefiles { "libuv/src/win/**" }

			links { "CoreFoundation.framework" }

			postbuildcommands
			{
				"cp -R %{wks.location}../libuv/include/ %{wks.location}../inc/libuv/"
			}

		-- -----------------------------------------------------------------------
		-- VSCode / gmake2 (Android NDK)
		-- Android는 epoll 기반, pthread 링크
		-- -----------------------------------------------------------------------
		filter { "action:gmake2" }
			system       "android"
			buildoptions
			{
				"--sysroot=$(ANDROID_NDK)/toolchains/llvm/prebuilt/linux-x86_64/sysroot",
				"-target aarch64-linux-android21"
			}
			defines
			{
				"_GNU_SOURCE",          -- GNU 확장 활성화
				"ANDROID"
			}

			files
			{
				"libuv/include/**.h",
				"libuv/src/*.h",
				"libuv/src/*.c",
				"libuv/src/unix/**.h",
				"libuv/src/unix/**.c"
			}

			removefiles { "libuv/src/win/**" }

			links { "pthread", "dl" }

			postbuildcommands
			{
				"cp -R %{wks.location}../libuv/include/ %{wks.location}../inc/libuv/"
			}

		filter {}
	end

	-- ==========================================================================
	-- 프로젝트: ufbx (FBX 파일 파서)
	-- ==========================================================================
	project "ufbx"
	do
		kind "StaticLib"

		files
		{
			"ufbx/**.h",
			"ufbx/**.c"
		}

		includedirs { "ufbx" }

		targetdir (outputdir)
		objdir    (objoutdir)

		filter "configurations:Debug"
			targetname "ufbx_d"

		filter "configurations:Release"
			targetname "ufbx"

		-- -----------------------------------------------------------------------
		-- Visual Studio (Windows)
		-- -----------------------------------------------------------------------
		filter { "action:vs*" }
			system       "Windows"
			buildoptions { "/utf-8" }
			defines      { "_CRT_SECURE_NO_WARNINGS" }

			postbuildcommands
			{
				"echo f | xcopy /I /Y $(ProjectDir)..\\ufbx\\*.h $(ProjectDir)..\\inc\\ufbx\\*.h"
			}

		-- -----------------------------------------------------------------------
		-- Xcode (macOS)
		-- -----------------------------------------------------------------------
		filter { "action:xcode*", "system:macosx" }
			system       "macosx"
			buildoptions { "-mmacosx-version-min=11.0" }

			postbuildcommands
			{
				"cp -R %{wks.location}../ufbx/*.h %{wks.location}../inc/ufbx/"
			}

		-- -----------------------------------------------------------------------
		-- Xcode (iOS)
		-- -----------------------------------------------------------------------
		filter { "action:xcode*", "system:ios" }
			system       "ios"
			buildoptions { "-mios-version-min=14.0" }

			postbuildcommands
			{
				"cp -R %{wks.location}../ufbx/*.h %{wks.location}../inc/ufbx/"
			}

		-- -----------------------------------------------------------------------
		-- VSCode / gmake2 (Android NDK)
		-- -----------------------------------------------------------------------
		filter { "action:gmake2" }
			system       "android"
			buildoptions
			{
				"--sysroot=$(ANDROID_NDK)/toolchains/llvm/prebuilt/linux-x86_64/sysroot",
				"-target aarch64-linux-android21"
			}

			postbuildcommands
			{
				"cp -R %{wks.location}../ufbx/*.h %{wks.location}../inc/ufbx/"
			}

		filter {}
	end

end

-- =============================================================================
-- 사용법 (Usage)
-- =============================================================================
-- [Windows - Visual Studio 2022]
--   premake5 vs2022
--
-- [macOS - Xcode]
--   premake5 --os=macosx xcode4
--
-- [iOS - Xcode]
--   premake5 --os=ios xcode4
--
-- [Android - VSCode / Makefile (NDK 필요)]
--   export ANDROID_NDK=/path/to/your/ndk
--   premake5 --os=android gmake2
--   make config=debug_arm64
-- =============================================================================