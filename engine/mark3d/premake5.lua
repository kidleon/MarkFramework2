project("mark3d")
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
		"core/**.h",
		"core/**.inl",
		"core/**.cpp",
		"math/**.h",
		"math/**.inl",
		"math/**.cpp",
		"renderer/**.h",
		"renderer/**.inl",
		"renderer/**.cpp",
		"../common/*.h",
		"../common/generic/*.h",
		"../common/generic/*.cpp"
		
	}
	
	includedirs { 
		".",
		"../common",
		"../common/generic",
		"../baselib/public",
		"math",
		"core/public",
		"core/private",
		"renderer/public",
		"renderer/private"
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
				"{COPY} %{prj.location}../engine/mark3d/renderer/public/*.h %{incoutputdir}",
				"{COPY} %{prj.location}../engine/mark3d/core/public/*.h %{incoutputdir}",
				"{COPY} %{prj.location}../engine/mark3d/math/*.h %{incoutputdir}",
				"{COPY} %{prj.location}../engine/mark3d/math/*.inl %{incoutputdir}",
				"{COPY} %{prj.location}../engine/common/predefine.h %{incoutputdir}",
				"{COPY} %{output_dir}/*.dll %{sdk_bin_dir}",
				"{COPY} %{output_dir}/*.lib %{sdk_lib_dir}",
				"{COPY} %{output_dir}/*.pdb %{sdk_sym_dir}",
				"{COPY} %{inc_output_dir}/*.h %{sdk_inc_dir}",
				"{COPY} %{inc_output_dir}/*.inl %{sdk_inc_dir}",
				"{COPY} %{output_dir}/*.dll %{sample_output_dir}",
			}
			
			defines { "__MARK3D_RENDERSYSTEM_D3D11__" }
			
			files {
				"**.cpp",
				"**.def"
			}
			
			filter "configurations:Debug"
			do
				defines{"DEBUG", "USE_DLL", "MARKENGINE_EXPORTS", "_CRT_SECURE_NO_WARNINGS"}
				symbols "On"
				targetname("mark3d_d")
				links { "baselib_d" }
			end
			
			filter "configurations:Release"
			do
				defines{"NDEBUG", "RELEASE", "USE_DLL", "MARKENGINE_EXPORTS", "_CRT_SECURE_NO_WARNINGS"}
				optimize "On"
				
				symbols "On"
				targetname("mark3d")
				links { "baselib" }
			end
			
			filter "configurations:Master"
			do
				defines{"NDEBUG", "MASTER", "USE_DLL", "MARKENGINE_EXPORTS", "_CRT_SECURE_NO_WARNINGS"}
				optimize "On"
				symbols "On"
				targetname("mark3d")
				links { "baselib" }
			end
		end
	end
	
end
