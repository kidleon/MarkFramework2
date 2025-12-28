workspace "MarkFramework2"
do
	platforms { "x64" }
	characterset ("MBCS")
	configurations { "Debug", "Release", "Master" }
	location "build"
	objoutdir = "%{wks.location}/immediate/%{prj.name}"
	objdir (objoutdir)
	
	sample_output_dir = "%{wks.location}/../../samples/output"
	
	include "engine/baselib"
	include "engine/mark3d"
	include "engine/renderer_d3d11"
	include "samples/sample_rendering"

end