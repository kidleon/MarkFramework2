project("Mark3D")
do
	language "C++"
	cppdialect "C++20"
	configurations { "Debug", "Release", "Master" }
	
	filter {} 
	
	filter {"action:vs*"}
	do
		platforms { "x64" }
		architecture "x86_64"
	end

	filter {"action:xcode*"}
	do
		platforms { "ARM64" }
		architecture "arm64"
		
	end
	
	sdk_bin_dir = "%{prj.location}/../Output/sdk/bin"
	sdk_lib_dir = "%{prj.location}/../Output/sdk/lib"
	sdk_sym_dir = "%{prj.location}/../Output/sdk/sym"
	sdk_inc_dir = "%{prj.location}/../Output/sdk/inc"
	output_dir = "%{prj.location}/../Output/%{prj.name}/bin"
	inc_output_dir = "%{prj.location}/../Output/%{prj.name}/inc"
	targetdir (output_dir)
	
	files { 
		"pch.h",
		"pch.cpp",
		"Core/**.h",
		"Core/**.inl",
		"Core/**.cpp",
		"Main/**.h",
		"Main/**.inl",
		"Main/**.cpp",
		"Renderer/**.h",
		"Renderer/**.inl",
		"Renderer/**.cpp",
		"../Common/**.h",
		"../Common/**.inl"
	}
	
	includedirs { 
		".",
		"Core",
		"Core/Private",
		"Core/Public",
		"Main",
		"Main/Private",
		"Main/Public",
		"Renderer",
		"Renderer/Private",
		"Renderer/Public",
		"../Common",
	}

	-- Windows(VS): PCH 사용
	filter { "action:vs*" }
		pchheader "pch.h"
		pchsource "pch.cpp"

	-- macOS(Xcode): PCH 컴파일 시 includedirs 가 전달되지 않는 문제로
	-- -include 플래그로 직접 강제 포함, -I 로 Common 경로 명시
	filter { "action:xcode*" }
		pchheader ""
		pchsource ""
		buildoptions {
			"-include %{wks.location}/../Engine/Mark3D/pch.h",
			"-I%{wks.location}/../Common"
		}

	filter {}

	-- =========================================================================
	-- Windows
	-- =========================================================================
	filter { "system:windows" }
	do
		externalincludedirs {
			"../External/inc",
			"../External/iconv",
			"../External/stb"
		}

		filter {"action:vs*"}
		do
			kind "SharedLib"
			platforms { "x64" }
			buildoptions { "/utf-8" }
			postbuildcommands
			{
				"{COPY} %{prj.location}../Engine/Common/*.h %{inc_output_dir}",
				"{COPY} %{prj.location}../Engine/Common/*.inl %{inc_output_dir}",
				"{COPY} %{prj.location}../Engine/Mark3D/Core/Public/*.h %{inc_output_dir}",
				"{COPY} %{prj.location}../Engine/Mark3D/Core/Public/*.inl %{inc_output_dir}",
				"{COPY} %{prj.location}../Engine/Mark3D/Main/Public/*.h %{inc_output_dir}",
				"{COPY} %{prj.location}../Engine/Mark3D/Main/Public/*.inl %{inc_output_dir}",
				"{COPY} %{prj.location}../Output/%{prj.name}/bin/*.dll %{sdk_bin_dir}",
				"{COPY} %{prj.location}../Output/%{prj.name}/bin/*.lib %{sdk_lib_dir}",
				"{COPY} %{prj.location}../Output/%{prj.name}/bin/*.pdb %{sdk_sym_dir}",
				"{COPY} %{inc_output_dir}/*.h %{sdk_inc_dir}",
				"{COPY} %{inc_output_dir}/*.inl %{sdk_inc_dir}",
				"{COPY} %{prj.location}../Output/%{prj.name}/bin/*.dll %{sample_output_dir}",
				"{COPY} %{prj.location}../Output/%{prj.name}/bin/*.lib %{sample_output_dir}",
			}
			
			libdirs {
				"../External/bin/%{cfg.platform}"
			}
			
			filter { "system:windows", "configurations:Debug" }
			do
				defines{"DEBUG", "USE_DLL", "MARKENGINE_EXPORTS", "__LOG_ENABLED__", "__MEMORY_TRACKER_ENABLED__", "__MOMORY_LIMIT_ENABLED__", "_CRT_SECURE_NO_WARNINGS"}
				optimize "Off"
				symbols "On"
				incrementallink "On"
				libdirs { 
					"../External/iconv/x64/DebugStatic"
				}
				links{"lz4_d", "ufbx_d", "libiconvStaticD", "spdlog" }
				targetname("Mark3D_d")
			end
			
			filter { "system:windows", "configurations:Release" }
			do
				defines{"NDEBUG", "RELEASE", "USE_DLL", "MARKENGINE_EXPORTS", "__LOG_ENABLED__", "__MEMORY_TRACKER_ENABLED__", "__MOMORY_LIMIT_ENABLED__", "_CRT_SECURE_NO_WARNINGS"}
				optimize "Full"
				symbols "On"
				incrementallink "Off"
				libdirs { 
					"../External/iconv/x64/ReleaseStatic"
				}
				links{"lz4", "ufbx", "libiconvStatic", "spdlog" }
				targetname("Mark3D")
			end
			
			filter { "system:windows", "configurations:Master" }
			do
				defines{"NDEBUG", "MASTER", "USE_DLL", "MARKENGINE_EXPORTS", "__MOMORY_LIMIT_ENABLED__", "_CRT_SECURE_NO_WARNINGS"}
				optimize "Full"
				symbols "On"
				incrementallink "Off"
				libdirs { 
					"../External/iconv/x64/ReleaseStatic"
				}
				links{"lz4", "ufbx", "libiconvStatic"}
				targetname("Mark3D")
			end
		end
	end

	filter {}  -- Windows 필터 초기화 (macOS 섹션으로 누출 방지)

	-- =========================================================================
	-- macOS
	-- =========================================================================
	filter { "action:xcode*", "system:macosx" }
	do
		system "macosx"
		systemversion "13.3"        -- -target 플래그와 충돌 방지

		externalincludedirs {
			"../External/inc",
			"../External/stb"
		}

		kind "SharedLib"
		
		buildoptions { "-mmacosx-version-min=13.3" }
		linkoptions  { "-mmacosx-version-min=13.3" }

		libdirs {
			"../External/bin/macos/universal"
		}

		links {
			"CoreFoundation.framework",
			"CoreServices.framework",
			"iconv",   -- macOS 시스템 내장 iconv
		}

		postbuildcommands
		{
			-- 출력 디렉토리 사전 생성 (없으면 cp 실패)
			"mkdir -p %{inc_output_dir}",
			"mkdir -p %{sdk_bin_dir}",
			"mkdir -p %{sdk_lib_dir}",
			"mkdir -p %{sdk_inc_dir}",
			"mkdir -p %{sample_output_dir}",
			-- 헤더 복사
			"cp -Rf %{prj.location}/../Engine/Common/*.h %{inc_output_dir}/",
			"cp -Rf %{prj.location}/../Engine/Common/*.inl %{inc_output_dir}/",
			"cp -Rf %{prj.location}/../Engine/Mark3D/Core/Public/*.h %{inc_output_dir}/",
			"cp -Rf %{prj.location}/../Engine/Mark3D/Main/Public/*.h %{inc_output_dir}/",
			-- 바이너리 복사
			"cp -Rf %{output_dir}/*.dylib %{sdk_bin_dir}/",
			"cp -Rf %{inc_output_dir}/*.h %{sdk_inc_dir}/",
			"cp -Rf %{inc_output_dir}/*.inl %{sdk_inc_dir}/",
			"cp -Rf %{output_dir}/*.dylib %{sample_output_dir}/",
		}

		filter { "system:macosx", "configurations:Debug" }
		do
			defines{"DEBUG", "USE_DLL", "MARKENGINE_EXPORTS", "__LOG_ENABLED__", "__MEMORY_TRACKER_ENABLED__", "__MEMORY_LIMIT_ENABLED__"}
			optimize "Off"
			symbols "On"
			links{"lz4_d", "ufbx_d", "spdlog_d"}
			targetname("Mark3D_d")
		end

		filter { "system:macosx", "configurations:Release" }
		do
			defines{"NDEBUG", "RELEASE", "USE_DLL", "MARKENGINE_EXPORTS", "__LOG_ENABLED__", "__MEMORY_TRACKER_ENABLED__", "__MEMORY_LIMIT_ENABLED__"}
			optimize "Full"
			symbols "On"
			links{"lz4", "ufbx", "spdlog"}
			targetname("Mark3D")
		end

		filter { "system:macosx", "configurations:Master" }
		do
			defines{"NDEBUG", "MASTER", "USE_DLL", "MARKENGINE_EXPORTS", "__MEMORY_LIMIT_ENABLED__"}
			optimize "Full"
			symbols "On"
			links{"lz4", "ufbx"}
			targetname("Mark3D")
		end
	end

end