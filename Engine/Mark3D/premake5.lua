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
	
	externalincludedirs {
		"../External/inc",
		"../External/iconv",
		"../External/stb"
	}
	
	pchheader "pch.h"
	pchsource "pch.cpp"
	
	filter { "system:windows" }
	do
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
				"{COPY} %{prj.location}../output/%{prj.name}/bin/*.dll %{sdk_bin_dir}",
				"{COPY} %{prj.location}../output/%{prj.name}/bin/*.lib %{sdk_lib_dir}",
				"{COPY} %{prj.location}../output/%{prj.name}/bin/*.pdb %{sdk_sym_dir}",
				"{COPY} %{inc_output_dir}/*.h %{sdk_inc_dir}",
				"{COPY} %{inc_output_dir}/*.inl %{sdk_inc_dir}",
				"{COPY} %{prj.location}../output/%{prj.name}/bin/*.dll %{sample_output_dir}",
				"{COPY} %{prj.location}../output/%{prj.name}/bin/*.lib %{sample_output_dir}",
			}
			
			libdirs {
				"../External/bin/%{cfg.platform}"
			}
			
			filter "configurations:Debug"
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
			
			filter "configurations:Release"
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
			
			filter "configurations:Master"
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
	
end
