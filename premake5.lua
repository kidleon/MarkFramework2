workspace "MarkFramework2"
do
	characterset ("MBCS")
	configurations { "Debug", "Release", "Master" }
	location "Build"
	objoutdir = "%{wks.location}/Intermediate/%{prj.name}"
	objdir (objoutdir)

	filter {"action:vs*"}
		platforms { "x64" }
		architecture "x86_64"
		system "windows" 
	filter {}
	
	filter {"action:xcode*"}
		platforms { "ARM64" }
		architecture "arm64"
		system "macosx"
	filter {}

	sample_output_dir = "%{wks.location}/../Samples/Output"

	--
	--include "src/Rayun3D"
	--include "Samples/ECSTestApp"
	-- include "Engine/Renderer_D3D11"
	include "Engine/CoreService"
	include "Engine/Mark3D"
	--include "Samples/ConsoleApp"

end
