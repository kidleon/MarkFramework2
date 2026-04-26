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
		platforms { "Universal" }
		architecture "universal"
		buildoptions { "-arch arm64", "-arch x86_64" }

	filter {}

	sample_output_dir = "%{wks.location}/../Samples/Output"

	include "Engine/Mark3D"
	--include "src/Rayun3D"
	--include "Samples/ECSTestApp"
	-- include "engine/Renderer_D3D11"
	include "Samples/ConsoleApp"

end