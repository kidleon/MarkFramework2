project("baselib")
do
	language "C"
	cdialect "C17"
	
	sdk_bin_dir = "%{prj.location}/../output/sdk/bin"
	sdk_lib_dir = "%{prj.location}/../output/sdk/lib"
	sdk_sym_dir = "%{prj.location}/../output/sdk/sym"
	sdk_inc_dir = "%{prj.location}/../output/sdk/inc"
	outputdir = "%{prj.location}/../output/%{prj.name}/bin"
	incoutputdir = "%{prj.location}/../output/%{prj.name}/inc"
	targetdir (outputdir);
	
	files { 
		"*.h",
		"*.c",
		"private/**.h",
		"private/**.c",
		"public/**.h",
		"public/**.c",
		"../common/*.h"
	}
	
	includedirs { 
		".",
		"../common",
		"private",
		"public",
	}
	
	externalincludedirs {
		"../external/inc"
	}
	
	libdirs { 
		"../external/bin"
	}
	
	filter {} 
	
	filter "configurations:Debug"
	do
		links { "lz4_d" }
	end
	
	filter "configurations:Release"
	do
		links { "lz4" }
	end
		
	filter "configurations:Master"
	do
		links { "lz4" }
	end
	
	pchheader "pch.h"
	pchsource "pch.c"
	
	filter { "system:windows" }
	do
		filter {"action:vs*"}
		do
			kind "SharedLib"
			
			postbuildcommands
			{
				"{COPY} %{prj.location}../baselib/public/*.h %{incoutputdir}",
				"{COPY} %{prj.location}../common/predefine.h %{incoutputdir}",
				"{COPY} %{outputdir}/*.dll %{sdk_bin_dir}",
				"{COPY} %{outputdir}/*.lib %{sdk_lib_dir}",
				"{COPY} %{outputdir}/*.pdb %{sdk_sym_dir}",
				"{COPY} %{incoutputdir}/*.h %{sdk_inc_dir}",
				"{COPY} %{outputdir}/*.dll %{sample_output_dir}",
			}
			
			filter "configurations:Debug"
			do
				defines{"DEBUG", "USE_DLL", "MARK_BASELIB_EXPORTS", "_CRT_SECURE_NO_WARNINGS"}
				symbols "On"
				targetname("baselib_d")
			end
			
			filter "configurations:Release"
			do
				defines{"NDEBUG", "RELEASE", "USE_DLL", "MARK_BASELIB_EXPORTS", "_CRT_SECURE_NO_WARNINGS"}
				optimize "On"
				symbols "On"
				targetname("baselib") 
			end
				
			filter "configurations:Master"
			do
				defines{"NDEBUG", "MASTER", "USE_DLL", "MARK_BASELIB_EXPORTS", "_CRT_SECURE_NO_WARNINGS"}
				optimize "On"
				symbols "On"
				targetname("baselib")
			end
		end
	end
	
end

	
		
		
	
	

