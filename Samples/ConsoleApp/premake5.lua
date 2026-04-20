project("SampleConsole")
do
	--kind "WindowedApp"
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
		filter {"action:vs*"}
		do
			platforms { "x64" }
			buildoptions { "/utf-8" }
			filter {"configurations:Debug"}
			do
				links{"Mark3D_d"}
				defines{"DEBUG", "__MEMORY_TRACKER_ENABLED__", "__MOMORY_LIMIT_ENABLED__"}
				symbols "On"
				targetname("SampleConsole_d")
			end
			
			filter {"configurations:Release"}
			do
				defines{"NDEBUG", "RELEASE", "__MEMORY_TRACKER_ENABLED__", "__MOMORY_LIMIT_ENABLED__"}
				optimize "On"
				symbols "On"
				links{"Mark3D"}
				targetname("SampleConsole")
			end
			
			filter {"configurations:Master"}
			do
				defines{"NDEBUG", "MASTER" }
				optimize "On"
				symbols "On"
				links{"Mark3D"}
				targetname("SampleConsole")
				
			end
		end
	end
end