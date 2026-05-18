project("CoreService")
do
	language "C++"
	cppdialect "C++20"
	configurations { "Debug", "Release", "Master" }
	
	sdk_bin_dir = "%{prj.location}/../Output/sdk/bin"
	sdk_lib_dir = "%{prj.location}/../Output/sdk/lib"
	sdk_sym_dir = "%{prj.location}/../Output/sdk/sym"
	sdk_inc_dir = "%{prj.location}/../Output/sdk/inc"
	output_dir = "%{prj.location}/../Output/%{prj.name}/bin"
	inc_output_dir = "%{prj.location}/../Output/%{prj.name}/inc"
	targetdir (output_dir)
	
	files { 
		"../Common/**.h",
		"../Common/**.inl",
		"pch.h",
		"pch.cpp",
		"Private/**.h",
		"Private/**.cpp",
		"Public/**.h",
		"Public/**.cpp",
	}
	
	includedirs { 
		"../Common",
		".",
		"Private",
		"Public"
	}

	-- =========================================================================
	-- Windows
	-- =========================================================================
	filter { "action:vs*", "system:windows" }
	do
		kind "SharedLib"
		
		pchheader "pch.h"
		pchsource "pch.cpp"
		
		externalincludedirs {
			"../External/inc",
			"../External/stb",
		}

		buildoptions { "/utf-8", "/Zc:preprocessor" }
		
		postbuildcommands
		{
			"{COPY} %{prj.location}../Engine/Common/*.h %{inc_output_dir}",
			"{COPY} %{prj.location}../Engine/Common/*.inl %{inc_output_dir}",
			"{COPY} %{prj.location}../Engine/CoreService/Public/*.h %{inc_output_dir}",
			"{COPY} %{prj.location}../Engine/CoreService/Public/mtl/*.h %{inc_output_dir}",
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
		
			defines{"NOMINMAX", "WIN32_LEAN_AND_MEAN", "DEBUG", "USE_DLL", "MARKENGINE_EXPORTS", "__LOG_ENABLED__", "__MEMORY_TRACKER_ENABLED__", "_CRT_SECURE_NO_WARNINGS"}
			optimize "Off"
			symbols "On"
			incrementallink "On"
			links{"lz4_d", "ufbx_d", "libiconvd", "mimalloc_d", "spdlog" }
			
			targetname("CoreService_d")
		end
		filter {}
		
		filter { "system:windows", "configurations:Release" }
		do
			defines{"NOMINMAX", "WIN32_LEAN_AND_MEAN", "NDEBUG", "RELEASE", "USE_DLL", "MARKENGINE_EXPORTS", "__LOG_ENABLED__", "__MEMORY_TRACKER_ENABLED__", "_CRT_SECURE_NO_WARNINGS"}
			optimize "Full"
			symbols "On"
			incrementallink "Off"
			links{"lz4", "ufbx", "libiconv", "mimalloc", "spdlog" }
			targetname("CoreService")
		end
		filter {}
		
		filter { "system:windows", "configurations:Master" }
		do
			defines{"NOMINMAX", "WIN32_LEAN_AND_MEAN", "NDEBUG", "MASTER", "USE_DLL", "MARKENGINE_EXPORTS", "_CRT_SECURE_NO_WARNINGS"}
			optimize "Full"
			symbols "On"
			incrementallink "Off"
			links{"lz4", "ufbx", "libiconv", "mimalloc"}
			targetname("CoreService")
		end
		filter {}
	end

	filter {}  -- Windows 필터 초기화 (macOS 섹션으로 누출 방지)

	-- =========================================================================
	-- macOS
	-- =========================================================================
	filter { "action:xcode*", "system:macosx" }
	do
		systemversion "13.3"        -- -target 플래그와 충돌 방지
		
		externalincludedirs {
			"../External/inc",
			"../External/stb"
		}

		kind "SharedLib"
		
		pchheader ""
		pchsource ""
		buildoptions {
			"-include %{wks.location}/../Engine/CoreService/pch.h",
			"-I%{wks.location}/../Common"
		}
		
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
			"cp -Rf %{prj.location}/../Engine/CoreService/Public/*.h %{inc_output_dir}/",
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
			links{"lz4_d", "ufbx_d", "spdlog_d", "mimalloc_d"}
			targetname("CoreService_d")
		end
		filter {}

		filter { "system:macosx", "configurations:Release" }
		do
			defines{"NDEBUG", "RELEASE", "USE_DLL", "MARKENGINE_EXPORTS", "__LOG_ENABLED__", "__MEMORY_TRACKER_ENABLED__", "__MEMORY_LIMIT_ENABLED__"}
			optimize "Full"
			symbols "On"
			links{"lz4", "ufbx", "spdlog", "mimalloc"}
			targetname("CoreService")
		end
		filter {}

		filter { "system:macosx", "configurations:Master" }
		do
			defines{"NDEBUG", "MASTER", "USE_DLL", "MARKENGINE_EXPORTS", "__MEMORY_LIMIT_ENABLED__"}
			optimize "Full"
			symbols "On"
			links{"lz4", "ufbx", "mimalloc"}
			targetname("CoreService")
		end
		filter {}
	end
	filter {}

end