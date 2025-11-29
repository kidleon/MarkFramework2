project("core_app")
do
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	
	files { 
		"src/*.cpp",
		"src/*.inl",
		"src/*.h",
	}
	
	includedirs { 
		".",
		"src/*.h",
	}
	
	externalincludedirs {
		"%{sdk_inc_dir}"
	}
	
	libdirs { 
		"%{sdk_lib_dir}"
	}
	
	pchheader "pch.h"
	pchsource "src/pch.cpp"
	debugdir "%{ouput_dir}"
	
	ouput_dir = "%{wks.location}/../output"
	targetdir (ouput_dir);
	
	filter { "system:windows" }
	do
		filter {"action:vs*"}
		do
			filter {"configurations:Debug", "platforms:x64"}
			do
				defines{"DEBUG", "USE_DLL", "ENABLE_LOG"}
				links{"baselib_d", "mark3d_d"}
				symbols "On"
				
			end
			
			filter {"configurations:Release", "platforms:x64"}
			do
				defines{"NDEBUG", "RELEASE", "USE_DLL", "ENABLE_LOG"}
				optimize "On"
				symbols "On"
				links{"baselib", "mark3d"}
			end
			
			filter {"configurations:Master", "platforms:x64"}
			do
				defines{"NDEBUG", "MASTER", "USE_DLL", "ENABLE_LOG"}
				optimize "On"
				symbols "On"
				links{"baselib", "mark3d"}
			end
			
		end
	end
	
end
		
	
		
		
	
	

