workspace "MarkFramework2_Samples"
do
	platforms { "x64" }
	characterset ("MBCS")
	configurations { "Debug", "Release", "Master" }
	location "build"
	objoutdir = "%{wks.location}/immediate/%{prj.name}"
	objdir (objoutdir)
	
	ouput_dir = "%{wks.location}/../output"
	targetdir (outputdir);
	
	sdk_bin_dir = "%{wks.location}/../../engine/output/sdk/bin"
	sdk_inc_dir = "%{wks.location}/../../engine/output/sdk/inc"
	sdk_lib_dir = "%{wks.location}/../../engine/output/sdk/lib"
	sdk_sym_dir = "%{wks.location}/../../engine/output/sdk/sym"
	
	include "core_app"
end