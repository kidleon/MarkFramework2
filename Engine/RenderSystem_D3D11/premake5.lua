project("RenderSystem_D3D11")
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
	}
	
	includedirs { 
		"../Common",
		"../CoreService/public",
		".",
		"Private"
	}
	
	filter {} 
	
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
		
		files {
			"dllmain.cpp",
			"RenderSystem_D3D11.def"
		}

		buildoptions { "/utf-8", "/Zc:preprocessor" }
		
		postbuildcommands
		{
			"{COPY} %{prj.location}../Engine/Common/*.h %{inc_output_dir}",
			"{COPY} %{prj.location}../Engine/Common/*.inl %{inc_output_dir}",
			"{COPY} %{prj.location}../Output/%{prj.name}/bin/*.dll %{sdk_bin_dir}",
			"{COPY} %{prj.location}../Output/%{prj.name}/bin/*.lib %{sdk_lib_dir}",
			"{COPY} %{prj.location}../Output/%{prj.name}/bin/*.pdb %{sdk_sym_dir}",
			"{COPY} %{inc_output_dir}/*.h %{sdk_inc_dir}",
			"{COPY} %{inc_output_dir}/*.inl %{sdk_inc_dir}",
			"{COPY} %{prj.location}../Output/%{prj.name}/bin/*.dll %{sample_output_dir}",
			"{COPY} %{prj.location}../Output/%{prj.name}/bin/*.lib %{sample_output_dir}",
		}
		
		libdirs {
			"../External/bin/%{cfg.platform}",
			"../../Output/CoreService/bin"
		}
		
		filter { "system:windows", "configurations:Debug" }
		do
		
			defines{"NOMINMAX", "WIN32_LEAN_AND_MEAN", "DEBUG", "USE_DLL", "MARKENGINE_EXPORTS", "__LOG_ENABLED__", "__MEMORY_TRACKER_ENABLED__", "_CRT_SECURE_NO_WARNINGS"}
			optimize "Off"
			symbols "On"
			incrementallink "On"
			links{ "CoreService_d" }
			
			targetname("RenderSystem_D3D11d")
		end
		filter {}
		
		filter { "system:windows", "configurations:Release" }
		do
			defines{"NOMINMAX", "WIN32_LEAN_AND_MEAN", "NDEBUG", "RELEASE", "USE_DLL", "MARKENGINE_EXPORTS", "__LOG_ENABLED__", "__MEMORY_TRACKER_ENABLED__", "_CRT_SECURE_NO_WARNINGS"}
			optimize "Full"
			symbols "On"
			incrementallink "Off"
			links{ "CoreService" }
			targetname("RenderSystem_D3D11")
		end
		filter {}
		
		filter { "system:windows", "configurations:Master" }
		do
			defines{"NOMINMAX", "WIN32_LEAN_AND_MEAN", "NDEBUG", "MASTER", "USE_DLL", "MARKENGINE_EXPORTS", "_CRT_SECURE_NO_WARNINGS"}
			optimize "Full"
			symbols "On"
			incrementallink "Off"
			links{ "CoreService" }
			targetname("RenderSystem_D3D11")
		end
		filter {}
	end

	filter {}  -- Windows 필터 초기화 (macOS 섹션으로 누출 방지)
	
end
