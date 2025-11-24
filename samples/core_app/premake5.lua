project("core_app")
do
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	location "build"
	files { 
		"src/*.cpp",
		"src/*.inl",
		"src/*.h",
	}
	
	includedirs { 
		".",
		"%{wks.location}/../include",
	}
	
	configurations { "Debug", "Release" }
	platforms { "x64" }
	 
	filter "configurations:Debug"
		defines{"DEBUG", "USE_DLL", "ENABLE_LOG"}
		symbols "On"
		
	filter "configurations:Release"
		defines{"NDEBUG", "USE_DLL", "ENABLE_LOG"}
		optimize "On"
		symbols "On"
	
	
	
	pchheader "pch.h"
	pchsource "src/pch.cpp"

	filter { "system:windows" }
	do
		filter {"action:vs*"}
		do
			characterset ("MBCS")
			
			-- SETUP DIRECTORIES
			objoutdir = "%{prj.location}/immediate/"
			outputdir = "%{prj.location}/../../bin"
			targetdir (outputdir);
			objdir (objoutdir)
			libdirs
			{
				"%{wks.location}/../bin"
			}
			debugdir "%{prj.location}/../../bin"
			postbuildcommands
			{
				-- "echo f | xcopy /I /Y $(ProjectDir)Output\\$(TargetName).exe $(ProjectDir)..\\Output\\Binaries\\$(TargetName).exe",
				-- "echo f | xcopy /I /Y $(ProjectDir)Output\\$(TargetName).pdb $(ProjectDir)..\\Output\\Binaries\\$(TargetName).pdb"
			}
			
			filter {"configurations:Debug", "platforms:x64"}
				links("MarkFramework_d")
			
			filter {"configurations:Release", "platforms:x64"}
				links("MarkFramework")
			
		end
	end
	
end
		
	
		
		
	
	

