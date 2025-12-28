project("sample_rendering")
do
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	
	sdk_bin_dir = "%{prj.location}/../output/sdk/bin"
	sdk_lib_dir = "%{prj.location}/../output/sdk/lib"
	sdk_sym_dir = "%{prj.location}/../output/sdk/sym"
	sdk_inc_dir = "%{prj.location}/../output/sdk/inc"
	outputdir = "%{prj.location}/../output/%{prj.name}/bin"
	incoutputdir = "%{prj.location}/../output/%{prj.name}/inc"
	targetdir (outputdir);
	
	files { 
		"src/*.cpp",
		"src/*.inl",
		"src/*.h",
	}
	
	includedirs { 
		".",
		"src/*.h"
	}
	
	externalincludedirs {
		"%{sdk_inc_dir}"
	}
	
	libdirs { 
		"%{sdk_lib_dir}"
	}
	
	pchheader "pch.h"
	pchsource "src/pch.cpp"
	debugdir "%{sdk_bin_dir}"
	
	filter { "system:windows" }
	do
		filter {"action:vs*"}
		do
			filter {"configurations:Debug", "platforms:x64"}
			do
				defines{"DEBUG", "USE_DLL", "ENABLE_LOG"}
				links{"baselib_d", "mark3d_d"}
				symbols "On"
				targetname("sample_rendering_d")
			end
			
			filter {"configurations:Release", "platforms:x64"}
			do
				defines{"NDEBUG", "RELEASE", "USE_DLL", "ENABLE_LOG"}
				optimize "On"
				symbols "On"
				links{"baselib", "mark3d"}
				targetname("sample_rendering")
			end
			
			filter {"configurations:Master", "platforms:x64"}
			do
				defines{"NDEBUG", "MASTER", "USE_DLL", "ENABLE_LOG"}
				optimize "On"
				symbols "On"
				links{"baselib", "mark3d"}
				targetname("sample_rendering")
			end
		end
	end
end