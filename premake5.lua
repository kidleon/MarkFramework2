workspace "MarkFramework2"
do
	-- SETUP CONFIGURATIONS
	language "C++"
	cppdialect "C++17"
	configurations { "Debug", "Release", "Master" }
	platforms { "x64" }
	characterset ("MBCS")
	location "build"
	
	filter "configurations:Debug"
		defines{"DEBUG", "ENABLE_LOG", "USE_PROFILE_MEMORY"}
		symbols "On"
		
	filter "configurations:Release"
		defines{"NDEBUG", "RELEASE", "ENABLE_LOG", "USE_PROFILE_MEMORY"}
		optimize "On"
		symbols "On"
		
	filter "configurations:Master"
		defines{"NDEBUG", "RELEASE", "MASTER", "ENABLE_LOG"}
		optimize "On"
		symbols "On"
	
	-- SETUP DIRECTORIES
	objoutdir = "%{wks.location}/Immediate/"
	outputdir = "%{wks.location}/../bin"

	project "MarkFramework2"
	do
		libdirs 
		{
			"external/bin"
		}
		
		links 
		{
			"lz4"
		}
		
		pchheader "pch.h"
		pchsource "src/pch.cpp"
		
		debugdir (runtimedir)
		targetdir (outputdir)
		objdir (objoutdir)
		filter "configurations:Debug"
			targetname("MarkFramework_d")
		filter "configurations:Release"
			targetname("MarkFramework")
		filter "configurations:Master"
			targetname("MarkFramework")
			
		filter {"action:vs*"}
		do
			
			filter "platforms:x64"
				system "Windows"
				architecture "x64"
		
			kind "SharedLib"
			buildoptions { "/utf-8" }
			defines
			{ 
				"_CRT_SECURE_NO_WARNINGS", 
				"RL_GPUTEX_IMPLEMENTATION", 
				"RL_GPUTEX_SUPPORT_DDS", 
				"USE_DLL", 
				"MARKENGINE_EXPORTS" 
			}
			
			-- SETUP SOURCE FILES
			files { 
				"src/**.h",
				"src/**.cpp",
				"src/common/**.h",
				"src/baselib/private/**.h", 
				"src/baselib/private/**.inl", 
				"src/baselib/private/**.cpp", 
				"src/baselib/private/**.c", 
				"src/baselib/private/**.h", 
				"src/baselib/public/**.h", 
				"src/baselib/public/**.inl", 
				"src/baselib/public/**.cpp", 
				"src/baselib/public/**.c", 
				"src/baselib/public/**.h",
				"src/generic/**.h", 
				"src/math/**.h", 
				"src/math/**.inl", 
				"src/renderer/private/**.h", 
				"src/renderer/private/**.inl", 
				"src/renderer/private/**.cpp", 
				"src/renderer/private/**.c", 
				"src/renderer/private/**.h", 
				"src/renderer/public/**.h", 
				"src/renderer/public/**.inl", 
				"src/renderer/public/**.cpp", 
				"src/renderer/public/**.c", 
				"src/renderer/public/**.h",
			}
			
			includedirs { 
				"external/inc",
				"src",
				"src/common",
				"src/baselib/private",
				"src/baselib/public",
				"src/core/private",
				"src/core/public",
				"src/generic",
				"src/math",
				"src/renderer",
				"src/renderer/private",
				"src/renderer/public",
			}
			
			postbuildcommands
			{
				-- "echo Project Directory: $(ProjectDir)",
				-- "dir $(ProjectDir)..\\src\\common\\",
				"echo f | xcopy /I /Y $(ProjectDir)..\\src\\common\\*.h $(ProjectDir)..\\include\\*.h",
				"echo f | xcopy /I /Y $(ProjectDir)..\\src\\baselib\\public\\*.h $(ProjectDir)..\\include\\*.h",
				"echo f | xcopy /I /Y $(ProjectDir)..\\src\\core\\public\\*.h $(ProjectDir)..\\include\\*.h",
				--"echo f | xcopy /I /Y $(ProjectDir)..\\src\\generic\\*.h $(ProjectDir)..\\include\\*.h",
				--"echo f | xcopy /I /Y $(ProjectDir)..\\src\\generic\\*.inl $(ProjectDir)..\\include\\*.inl",
				--"echo f | xcopy /I /Y $(ProjectDir)..\\src\\math\\*.h $(ProjectDir)..\\include\\*.h",
				--"echo f | xcopy /I /Y $(ProjectDir)..\\src\\math\\*.inl $(ProjectDir)..\\include\\*.inl",
				
				"echo f | xcopy /I /Y $(ProjectDir)..\\bin\\MarkFramework_d.dll $(ProjectDir)..\\samples\\bin\\MarkFramework_d.dll",
				"echo f | xcopy /I /Y $(ProjectDir)..\\bin\\MarkFramework.dll $(ProjectDir)..\\samples\\bin\\MarkFramework.dll",
			}
		end
	end
	
	include "samples/core_app"
end