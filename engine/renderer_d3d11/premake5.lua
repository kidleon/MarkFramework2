project("renerer_d3d11")
do
	language "C++"
	cppdialect "C++17"
	platforms { "x64" }
	
	sdk_bin_dir = "%{prj.location}/../output/sdk/bin"
	sdk_lib_dir = "%{prj.location}/../output/sdk/lib"
	sdk_sym_dir = "%{prj.location}/../output/sdk/sym"
	sdk_inc_dir = "%{prj.location}/../output/sdk/inc"
	output_dir = "%{prj.location}/../output/%{prj.name}/bin"
	inc_output_dir = "%{prj.location}/../output/%{prj.name}/inc"
	targetdir (output_dir)
	
	files { 
		"pch.h",
		"pch.cpp",
		"public/**.h",
		"public/**.inl",
		"public/**.cpp",
		"private/**.h",
		"private/**.inl",
		"private/**.cpp",
		"../common/*.h",
		"../common/generic/*.h"
	}
	
	includedirs { 
		".",
		"../common",
		"../common/generic",
		"../baselib/public",
		"../mark3d/math",
		"../mark3d/core/public",
		"../mark3d/renderer/public",
		"public",
		"private"
	}
	
	externalincludedirs {
		"../external/inc",
		"../external/stb"
	}
	
	libdirs {
		"%{prj.location}/../output/sdk/lib"
	}
	
	pchheader "pch.h"
	pchsource "pch.cpp"
	
	filter {} 
	
	filter { "system:windows" }
	do
		filter {"action:vs*"}
		do
			kind "SharedLib"
			
			postbuildcommands
			{
				--"{COPY} %{prj.location}../renderer_d3d11/public/*.h %{incoutputdir}",
				--"{COPY} %{prj.location}../common/predefine.h %{incoutputdir}",
				"{COPY} %{prj.location}../output/%{prj.name}/bin/*.dll %{sdk_bin_dir}",
				-- "{COPY} %{output_dir}/*.lib %{sdk_lib_dir}",
				"{COPY} %{prj.location}../output/%{prj.name}/bin/*.pdb %{sdk_sym_dir}",
				--"{COPY} %{incoutputdir}/*.h %{sdk_inc_dir}",
				-- "{COPY} %{incoutputdir}/*.inl %{sdk_inc_dir}",
				"{COPY} %{prj.location}../output/%{prj.name}/bin/*.dll %{sample_output_dir}",
			}
			
			files {
				"**.cpp",
				"**.def"
			}
			
			filter "configurations:Debug"
			do
				defines{"DEBUG", "USE_DLL", "MARKENGINE_EXPORTS", "_CRT_SECURE_NO_WARNINGS"}
				symbols "On"
				targetname("renderer_d3d11_d")
				links { "baselib_d" }
			end
			
			filter "configurations:Release"
			do
				defines{"NDEBUG", "RELEASE", "USE_DLL", "MARKENGINE_EXPORTS", "_CRT_SECURE_NO_WARNINGS"}
				optimize "On"
				symbols "On"
				targetname("renderer_d3d11")
				links { "baselib" }
			end
			
			filter "configurations:Master"
			do
				defines{"NDEBUG", "MASTER", "USE_DLL", "MARKENGINE_EXPORTS", "_CRT_SECURE_NO_WARNINGS"}
				optimize "On"
				symbols "On"
				targetname("renderer_d3d11")
				links { "baselib" }
			end
		end
	end
	
end
