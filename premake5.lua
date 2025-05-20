
workspace "FrameEX"
	architecture "x64"

	startproject "FrameExtractor"

	configurations
	{
		"Debug",
		"Release",
		"Distribution"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

	IncludeDir = {}
	IncludeDir["GLFW"] = "FrameExtractor/lib/GLFW/include"
	IncludeDir["GLAD"] = "FrameExtractor/lib/GLAD/include"
	IncludeDir["ImGui"] = "FrameExtractor/lib/imgui"
	IncludeDir["GLM"] = "FrameExtractor/lib/glm"
	IncludeDir["FFMPEG"] = "FrameExtractor/lib/ffmpeg/include"
	IncludeDir["STBI"] = "FrameExtractor/lib/stb_image"
	IncludeDir["OpenXLSX"] = "FrameExtractor/lib/OpenXLSX/inc"
	IncludeDir["YAML"] = "FrameExtractor/lib/yaml-cpp/include"

	LibraryDir = {}
	LibraryDir["FFMPEG"] = "lib/ffmpeg/lib"
	LibraryDir["OpenXLSX"] = "lib/OpenXLSX/lib"

	Library = {}
	Library["AVUtil"] = "%{LibraryDir.FFMPEG}/avutil.lib"
	Library["AVCodec"] = "%{LibraryDir.FFMPEG}/avcodec.lib"
	Library["AVFormat"] = "%{LibraryDir.FFMPEG}/avformat.lib"
	Library["SWScale"] = "%{LibraryDir.FFMPEG}/swscale.lib"
	Library["OpenXLSX_Release"] = "%{LibraryDir.OpenXLSX}/OpenXLSX.lib"
	Library["OpenXLSX_Debug"] = "%{LibraryDir.OpenXLSX}/OpenXLSXd.lib"

	group "Dependencies"
		include "FrameExtractor/lib/GLAD/premake5.lua"
		include "FrameExtractor/lib/yaml-cpp/premake5.lua"
		include "FrameExtractor/lib/GLFW/premake5.lua"
		include "FrameExtractor/lib/IMGUI/premake5.lua"

	group ""


	project "FrameExtractor"
		location "FrameExtractor"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++20"
		staticruntime "on"
		systemversion "latest"

		targetdir ("build/" .. outputdir .. "/%{prj.name}")
		objdir ("build-int/" .. outputdir .. "/%{prj.name}")

		files
		{
			"%{prj.name}/**.hpp",
			"%{prj.name}/src/**.cpp",
			"%{prj.name}/lib/glm/glm/**.hpp",
			"%{prj.name}/lib/glm/glm/**.inl",
		}

		defines
		{
			"_CRT_SECURE_NO_WARNINGS"
		}

		includedirs
		{
			"%{prj.name}/inc",
			"%{prj.name}/lib/spdlog/include",
			"%{IncludeDir.GLFW}",
			"%{IncludeDir.GLAD}",
			"%{IncludeDir.ImGui}",
			"%{IncludeDir.GLM}",
			"%{IncludeDir.FFMPEG}",
			"%{IncludeDir.STBI}",
			"%{IncludeDir.OpenXLSX}",
			"%{IncludeDir.YAML}"
		}


		links
		{
			"opengl32.lib",
			"GLAD",
			"YAML",
			"GLFW",
			"ImGui"
		}


		defines
		{
			"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
			"GLFW_INCLUDE_NONE",
		}
		
		pchheader "FrameExtractorPCH.hpp"
		pchsource "FrameExtractor/src/FrameExtractorPCH.cpp"
		
		links
		{
				"%{Library.AVCodec}",
				"%{Library.AVUtil}",
				"%{Library.SWScale}",
				"%{Library.AVFormat}"
		}

		postbuildcommands {
				"{COPY} \"resources\" \"$(TargetDir)resources\"",
				"{COPYFILE} \"avcodec-62.dll\" \"$(TargetDir)avcodec-62.dll\"",
				"{COPYFILE} \"avformat-62.dll\" \"$(TargetDir)avformat-62.dll\"",
				"{COPYFILE} \"avutil-60.dll\" \"$(TargetDir)avutil-60.dll\"",
				"{COPYFILE} \"swresample-6.dll\" \"$(TargetDir)swresample-6.dll\"",
				"{COPYFILE} \"swscale-9.dll\" \"$(TargetDir)swscale-9.dll\"",
				"{COPYFILE} \"imgui.ini\" \"$(TargetDir)imgui.ini\""

		}

		filter "configurations:Debug"
			defines "_DEB"
			symbols "On"
			runtime "Debug"
			links
			{
				"%{Library.OpenXLSX_Debug}",
			}


		filter "configurations:Release"
			defines "_REL"
			optimize "On"
			runtime "Release"
			links
			{
				"%{Library.OpenXLSX_Release}",
			}

		filter "configurations:Distribution"
			defines "_DIST"
			optimize "On"
			runtime "Release"
			links
			{
				"%{Library.OpenXLSX_Release}",
			}
