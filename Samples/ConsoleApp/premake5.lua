project("SampleConsole")
do
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	
	sdk_bin_dir = "%{prj.location}/../output/sdk/bin"
	sdk_lib_dir = "%{prj.location}/../output/sdk/lib"
	sdk_sym_dir = "%{prj.location}/../output/sdk/sym"
	sdk_inc_dir = "%{prj.location}/../output/sdk/inc"
	output_dir = "%{sample_output_dir}"
	targetdir (output_dir);
	
	files { 
		"src/*.cpp",
		"src/*.inl",
		"src/*.h",
		"*.h"
	}
	
	includedirs { 
		".",
		"src"
	}
	
	externalincludedirs {
		"%{sdk_inc_dir}"
	}
	
	libdirs { 
		"%{sdk_lib_dir}",
	}
	
	pchheader "pch.h"
	pchsource "src/pch.cpp"
	debugdir "%{sample_output_dir}"
	
	filter { "system:windows" }
	do
		platforms { "x64" }
		architecture "x86_64"
		
		filter {"action:vs*"}
		do
			
			buildoptions { "/utf-8" }
			filter {"configurations:Debug"}
			do
				defines{"DEBUG", "USE_DLL", "__MEMORY_TRACKER_ENABLED__", "__MOMORY_LIMIT_ENABLED__", "__LOG_ENABLED__"}
				optimize "Off"
				symbols "On"
				links{"Mark3D_d"}
				targetname("SampleConsole_d")
			end
			
			filter {"configurations:Release"}
			do
				defines{"NDEBUG", "RELEASE", "USE_DLL", "__MEMORY_TRACKER_ENABLED__", "__MOMORY_LIMIT_ENABLED__", "__LOG_ENABLED__"}
				optimize "On"
				symbols "On"
				links{"Mark3D"}
				targetname("SampleConsole")
			end
			
			filter {"configurations:Master"}
			do
				defines{"NDEBUG", "MASTER", "USE_DLL" }
				optimize "On"
				symbols "On"
				links{"Mark3D"}
				targetname("SampleConsole")
			end
		end
	end
end