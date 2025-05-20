-- Define the project
project "YAML"
    kind "StaticLib" -- Default to static library, can be changed based on options
    language "C++"

    -- Set C++ standard
    cppdialect "C++20"
    defines { "YAML_CPP_STATIC_DEFINE" }
    
    -- Include directories
    includedirs {
        "include",
        "src"
    }
    
    -- Source files
    files {
        "src/*.cpp"
    }

    -- Set target properties
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")


    -- Test the build configuration
    filter "system:windows"
		systemversion "latest"
		cppdialect "C++20"
		staticruntime "On"

	filter "system:macosx"
		pic "On"
		systemversion "latest"
		cppdialect "C++20"
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"