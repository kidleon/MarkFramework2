project("SampleRender")
do
	kind "WindowedApp"
	language "C++"
	cppdialect "C++20"
	
	filter{}
	sdk_bin_dir = "%{prj.location}/../Output/sdk/bin"
	sdk_lib_dir = "%{prj.location}/../Output/sdk/lib"
	sdk_sym_dir = "%{prj.location}/../Output/sdk/sym"
	sdk_inc_dir = "%{prj.location}/../Output/sdk/inc"
	sc_sample_output_dir = "%{wks.location}/../Samples/Output"
	sc_output_dir = "%{wks.location}/../Samples/Output"
	targetdir (sc_output_dir)
	debugdir "%{sc_sample_output_dir}"

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
	
	
	-- Windows(VS): PCH 사용
	filter { "action:vs*" }
		pchheader "pch.h"
		pchsource "src/pch.cpp"
	filter {}

	-- macOS(Xcode): PCH 컴파일 시 includedirs 가 전달되지 않는 문제로
	-- -include 플래그로 직접 강제 포함, -I 로 Common 경로 명시
	filter { "action:xcode*" }
		pchheader ""
		pchsource ""
		buildoptions {
			"-include %{wks.location}/../Samples/ConsoleApp/pch.h",
			"-I%{wks.location}/../Engine/Common"
		}

	filter {}

	-- =========================================================================
	-- Windows
	-- =========================================================================
	filter { "action:vs*", "system:windows" }
	do
		platforms { "x64" }
		architecture "x86_64"
		buildoptions { "/utf-8", "/Zc:preprocessor" }

		filter { "system:windows", "configurations:Debug" }
		do
			defines{"DEBUG", "USE_DLL", "__MEMORY_TRACKER_ENABLED__", "__LOG_ENABLED__"}
			optimize "Off"
			symbols "On"
			links{"Mark3D_d", "CoreService_d"}
			targetname("SampleRender_d")
		end
		filter {}
		
		filter { "system:windows", "configurations:Release" }
		do
			defines{"NDEBUG", "RELEASE", "USE_DLL", "__MEMORY_TRACKER_ENABLED__", "__LOG_ENABLED__"}
			optimize "On"
			symbols "On"
			links{"Mark3D", "CoreService"}
			targetname("SampleRender")
		end
		filter {}
		
		filter { "system:windows", "configurations:Master" }
		do
			defines{"NDEBUG", "MASTER", "USE_DLL"}
			optimize "On"
			symbols "On"
			links{"Mark3D", "CoreService"}
			targetname("SampleRender")
		end
		filter {}
		
	end
	filter {}

	-- =========================================================================
	-- macOS
	-- =========================================================================
	filter { "action:xcode*", "system:macosx" }
	do
		system "macosx"
		systemversion "13.3"        -- -target 플래그와 충돌 방지
		dependson { "Mark3D" }      -- Mark3D 먼저 빌드되도록 의존성 설정

		buildoptions { "-mmacosx-version-min=13.3" }
		linkoptions  { "-mmacosx-version-min=13.3" }

		-- Mark3D.dylib 런타임 탐색 경로 (실행 파일과 같은 디렉토리)
		linkoptions  { "-rpath @executable_path" }

		filter { "system:macosx", "configurations:Debug" }
		do
			defines{"DEBUG", "USE_DLL", "__MEMORY_TRACKER_ENABLED__", "__LOG_ENABLED__"}
			optimize "Off"
			symbols "On"
			linkoptions { "-lMark3D_d", "-lCoreService_d" }
			targetname("SampleConsole_d")
		end
		filter{}	


		filter { "system:macosx", "configurations:Release" }
		do
			defines{"NDEBUG", "RELEASE", "USE_DLL", "__MEMORY_TRACKER_ENABLED__", "__LOG_ENABLED__"}
			optimize "On"
			symbols "On"
			linkoptions { "-lMark3D", "-lCoreService" }
			targetname("SampleConsole")
		end

		filter{}	

		filter { "system:macosx", "configurations:Master" }
		do
			defines{"NDEBUG", "MASTER", "USE_DLL"}
			optimize "On"
			symbols "On"
			linkoptions { "-lMark3D", "-lCoreService" }
			targetname("SampleConsole")
		end
	end
end
