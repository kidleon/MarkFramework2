workspace "MarkFramework2"
do
	platforms { "x64" }
	characterset ("MBCS")
	configurations { "Debug", "Release", "Master" }
	location "build"
	objoutdir = "%{wks.location}/immediate/%{prj.name}"
	objdir (objoutdir)
	
	sample_output_dir = "%{wks.location}/../../samples/output"
	
	include "baselib"
	include "mark3d"
	include "renderer_d3d11"
	--include "samples/core_app"

end