workspace "MarkFramework2"
do
	characterset ("MBCS")
	configurations { "Debug", "Release", "Master" }
	location "Build"
	objoutdir = "%{wks.location}/Intermediate/%{prj.name}"
	objdir (objoutdir)

	filter {"action:vs*"}
		platforms { "x64" }

	filter {"action:xcode*"}
		platforms { "ARM64" }
		architecture "arm64"
	
	filter {}

	sample_output_dir = "%{wks.location}/../Samples/Output"

	include "Engine/Mark3D"
	--include "src/Rayun3D"
	--include "Samples/ECSTestApp"
	-- include "Engine/Renderer_D3D11"
	filter {}
	include "Samples/ConsoleApp"

end
