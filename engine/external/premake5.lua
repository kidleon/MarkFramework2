workspace "MarkFrameworkExternal"
do
	-- SETUP CONFIGURATIONS
	language "C"
	configurations { "Debug", "Release" }
	platforms { "x64" }
	characterset ("MBCS")
	location "build"
	
	filter "configurations:Debug"
		defines{"DEBUG"}
		symbols "On"
		
	filter "configurations:Release"
		defines{"NDEBUG", "RELEASE"}
		optimize "On"
		symbols "On"
	
	-- SETUP DIRECTORIES
	objoutdir = "%{wks.location}/Immediate/"
	outputdir = "%{wks.location}/../bin"

	project "lz4"
	do
		filter {"action:vs*"}
		do
			filter "platforms:x64"
				system "Windows"
				architecture "x64"
		
			kind "StaticLib"
			buildoptions { "/utf-8" }
			defines{ 
				"_CRT_SECURE_NO_WARNINGS", 
			}
			
			-- SETUP SOURCE FILES
			files { 
				"lz4/**.h",
				"lz4/**.c"
			}
			
			includedirs { 
				"lz4"
			}
			
			postbuildcommands
			{
				"echo f | xcopy /I /Y $(ProjectDir)..\\lz4\\*.h $(ProjectDir)..\\inc\\lz4\\*.h"
			}
			
			debugdir (runtimedir)
			targetdir (outputdir)
			objdir (objoutdir)
			
			filter "configurations:Debug"
				targetname("lz4_d")
		
			filter "configurations:Release"
				targetname("lz4")
			
		end
	end
	
	project "libuv"
	do
		filter {"action:vs*"}
		do
			filter "platforms:x64"
				system "Windows"
				architecture "x64"
		
			kind "StaticLib"
			buildoptions { "/utf-8" }
			defines{ 
				"_CRT_SECURE_NO_WARNINGS",
				"WIN32_LEAN_AND_MEAN",
				"_WIN32_WINNT=0x0600"
			}
			
			-- SETUP SOURCE FILES
			files { 
				"libuv/include/**.h",
				"libuv/src/*.h",
				"libuv/src/*.c",
				"libuv/src/win/**.h",
				"libuv/src/win/**.c"
			}
			
			-- Unix 파일 제외
			removefiles {
				"libuv/src/unix/**"
			}
			
			includedirs { 
				"libuv/include",
				"libuv/src"
			}
			
			links {
				"ws2_32",
				"psapi",
				"iphlpapi",
				"userenv",
				"advapi32"
			}
			
			postbuildcommands
			{
				"echo f | xcopy /I /Y /S $(ProjectDir)..\\libuv\\include\\*.* $(ProjectDir)..\\inc\\libuv\\*.*"
			}
			
			debugdir (runtimedir)
			targetdir (outputdir)
			objdir (objoutdir)
			filter "configurations:Debug"
				targetname("libuv_d")
		
			filter "configurations:Release"
				targetname("libuv")
		end
	end
end