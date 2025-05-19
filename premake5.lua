
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
	LibraryDir["GLAD_Debug"] = "lib/GLAD/lib/Deb"
	LibraryDir["GLAD_Release"] = "lib/GLAD/lib/Rel"
	LibraryDir["GLFW_Debug"] = "lib/GLFW/lib/Deb"
	LibraryDir["GLFW_Release"] = "lib/GLFW/lib/Rel"
	LibraryDir["IMGUI_Debug"] = "lib/IMGUI/lib/Deb"
	LibraryDir["IMGUI_Release"] = "lib/IMGUI/lib/Rel"
	LibraryDir["FFMPEG"] = "lib/ffmpeg/lib"
	LibraryDir["OpenXLSX"] = "lib/OpenXLSX/lib"
	LibraryDir["YAML_Debug"] = "lib/yaml-cpp/lib/Deb"
	LibraryDir["YAML_Release"] = "lib/yaml-cpp/lib/Rel"

	Library = {}
	Library["GLAD_Debug"] = "%{LibraryDir.GLAD_Debug}/GLAD.lib"
	Library["GLAD_Release"] = "%{LibraryDir.GLAD_Release}/GLAD.lib"
	Library["GLFW_Debug"] = "%{LibraryDir.GLFW_Debug}/GLFW.lib"
	Library["GLFW_Release"] = "%{LibraryDir.GLFW_Release}/GLFW.lib"
	Library["IMGUI_Debug"] = "%{LibraryDir.IMGUI_Debug}/IMGUI.lib"
	Library["IMGUI_Release"] = "%{LibraryDir.IMGUI_Release}/IMGUI.lib"
	Library["AVUtil"] = "%{LibraryDir.FFMPEG}/avutil.lib"
	Library["AVCodec"] = "%{LibraryDir.FFMPEG}/avcodec.lib"
	Library["AVFormat"] = "%{LibraryDir.FFMPEG}/avformat.lib"
	Library["SWScale"] = "%{LibraryDir.FFMPEG}/swscale.lib"
	Library["OpenXLSX_Release"] = "%{LibraryDir.OpenXLSX}/OpenXLSX.lib"
	Library["OpenXLSX_Debug"] = "%{LibraryDir.OpenXLSX}/OpenXLSXd.lib"
	Library["YAML_Debug"] = "%{LibraryDir.YAML_Debug}/yaml-cpp.lib"
	Library["YAML_Release"] = "%{LibraryDir.YAML_Release}/yaml-cpp.lib"

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
				"%{Library.GLAD_Debug}",
				"%{Library.GLFW_Debug}",
				"%{Library.IMGUI_Debug}",
				"%{Library.OpenXLSX_Debug}",
				"%{Library.YAML_Debug}"
			}


		filter "configurations:Release"
			defines "_REL"
			optimize "On"
			runtime "Release"
			links
			{
				"%{Library.GLAD_Release}",
				"%{Library.GLFW_Release}",
				"%{Library.IMGUI_Release}",
				"%{Library.OpenXLSX_Release}",
				"%{Library.YAML_Release}"
			}

		filter "configurations:Distribution"
			defines "_DIST"
			optimize "On"
			runtime "Release"
			links
			{
				"%{Library.GLAD_Release}",
				"%{Library.GLFW_Release}",
				"%{Library.IMGUI_Release}",
				"%{Library.OpenXLSX_Release}",
				"%{Library.YAML_Release}"
			}
