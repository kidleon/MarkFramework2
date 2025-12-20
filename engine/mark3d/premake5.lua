project("mark3d")
do
	language "C++"
	cppdialect "C++17"
	platforms { "x64" }
	
	sdk_bin_dir = "%{prj.location}/../output/sdk/bin"
	sdk_lib_dir = "%{prj.location}/../output/sdk/lib"
	sdk_sym_dir = "%{prj.location}/../output/sdk/sym"
	sdk_inc_dir = "%{prj.location}/../output/sdk/inc"
	outputdir = "%{prj.location}/../output/%{prj.name}/bin"
	incoutputdir = "%{prj.location}/../output/%{prj.name}/inc"
	targetdir (outputdir);
	
	files { 
		"pch.h",
		"pch.cpp",
		"public/**.h",
		"public/**.inl",
		"public/**.cpp",
		"private/**.h",
		"private/**.inl",
		"private/**.cpp",
		"../common/*.h"
	}
	
	includedirs { 
		".",
		"../common",
		"private",
		"public",
		"generic",
		"math",
	}
	
	externalincludedirs {
		"../external/inc",
		"../external/stb",
		"%{prj.location}/../output/sdk/inc"
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
				"{COPY} %{prj.location}../mark3d/public/*.h %{incoutputdir}",
				"{COPY} %{prj.location}../common/predefine.h %{incoutputdir}",
				"{COPY} %{outputdir}/*.dll %{sdk_bin_dir}",
				"{COPY} %{outputdir}/*.lib %{sdk_lib_dir}",
				"{COPY} %{outputdir}/*.pdb %{sdk_sym_dir}",
				"{COPY} %{incoutputdir}/*.h %{sdk_inc_dir}",
				"{COPY} %{incoutputdir}/*.inl %{sdk_inc_dir}",
				"{COPY} %{outputdir}/*.dll %{sample_output_dir}",
			}
			
			defines { "__MARK3D_RENDERSYSTEM_D3D11__" }
			
			files {
				"D3D11/**.h",
				"D3D11/**.inl",
				"D3D11/**.cpp",
				"**.cpp",
				"**.def"
			}
			
			filter "configurations:Debug"
			do
				defines{"DEBUG", "USE_DLL", "MARKENGINE_EXPORTS", "_CRT_SECURE_NO_WARNINGS"}
				symbols "On"
				targetname("mark3d_d")
				links { "baselib_d", "corelib_d" }
			end
			
			filter "configurations:Release"
			do
				defines{"NDEBUG", "RELEASE", "USE_DLL", "MARKENGINE_EXPORTS", "_CRT_SECURE_NO_WARNINGS"}
				optimize "On"
				
				symbols "On"
				targetname("mark3d")
				links { "baselib", "corelib" }
			end
			
			filter "configurations:Master"
			do
				defines{"NDEBUG", "MASTER", "USE_DLL", "MARKENGINE_EXPORTS", "_CRT_SECURE_NO_WARNINGS"}
				optimize "On"
				symbols "On"
				targetname("mark3d")
				links { "baselib", "corelib" }
			end
		end
	end
	
end
