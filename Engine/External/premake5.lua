workspace "MarkFrameworkExternal"
do
	-- SETUP CONFIGURATIONS
	language "C"
	configurations { "Debug", "Release" }
	
	filter {"action:vs*"}
	do
		platforms { "x64" }
		architecture "x86_64"
	end
	
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
	outputdir = "%{wks.location}/../bin/%{cfg.platform}"
	
	project "lz4"
	do
		-- SETUP SOURCE FILES
		files 
		{ 
			"lz4/**.h",
			"lz4/**.c"
		}
		
		includedirs 
		{
			"lz4"
		}
		
		debugdir (runtimedir)
		targetdir (outputdir)
		objdir (objoutdir)
		
		filter "configurations:Debug"
			targetname("lz4_d")
	
		filter "configurations:Release"
			targetname("lz4")
		
		filter {"action:vs*"}
		
		do
			system "Windows"
			kind "StaticLib"
			buildoptions { "/utf-8" }
			defines
			{
				"_CRT_SECURE_NO_WARNINGS", 
			}
			
			postbuildcommands
			{
				"echo f | xcopy /I /Y $(ProjectDir)..\\lz4\\*.h $(ProjectDir)..\\inc\\lz4\\*.h"
			}
		end
	end
	
	project "libuv"
	do
		-- SETUP SOURCE FILES
		includedirs 
		{ 
			"libuv/include",
			"libuv/src"
		}
		
		debugdir (runtimedir)
		targetdir (outputdir)
		objdir (objoutdir)
		
		filter "configurations:Debug"
			targetname("libuv_d")
	
		filter "configurations:Release"
			targetname("libuv")
		
		filter {"action:vs*"}
		do
			system "Windows"
		
			kind "StaticLib"
			buildoptions { "/utf-8" }
			defines
			{ 
				"_CRT_SECURE_NO_WARNINGS",
				"WIN32_LEAN_AND_MEAN",
				"_WIN32_WINNT=0x0600"
			}
			
			postbuildcommands
			{
				"echo f | xcopy /I /Y /S $(ProjectDir)..\\libuv\\include\\*.* $(ProjectDir)..\\inc\\libuv\\*.*"
			}
			
			files 
			{ 
				"libuv/include/**.h",
				"libuv/src/*.h",
				"libuv/src/*.c",
				"libuv/src/win/**.h",
				"libuv/src/win/**.c"
			}
			
			links 
			{
				"ws2_32",
				"psapi",
				"iphlpapi",
				"userenv",
				"advapi32"
			}
			
			-- Unix 파일 제외
			removefiles 
			{
				"libuv/src/unix/**"
			}
		end
	end
	
	project "ufbx"
	do
		kind "StaticLib"
		
		-- SETUP SOURCE FILES
		files 
		{ 
			"ufbx/**.h",
			"ufbx/**.c"
		}
		
		includedirs 
		{ 
			"ufbx"
		}
		
		debugdir (runtimedir)
		targetdir (outputdir)
		objdir (objoutdir)
		
		filter "configurations:Debug"
			targetname("ufbx_d")
	
		filter "configurations:Release"
			targetname("ufbx")
	
		filter {"action:vs*"}
		
		do
			system "Windows"
			
			buildoptions { "/utf-8" }
			defines
			{ 
				"_CRT_SECURE_NO_WARNINGS", 
			}
			
			postbuildcommands
			{
				"echo f | xcopy /I /Y $(ProjectDir)..\\ufbx\\*.h $(ProjectDir)..\\inc\\ufbx\\*.h"
			}
		end
	end
end