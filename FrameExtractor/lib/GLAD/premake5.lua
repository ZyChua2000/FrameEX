project "GLAD"
    kind "StaticLib"
    language "C"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    -- Common GLFW source files
    files
    {
        "include/glad/glad.h",
        "include/khr/khrplatform.h",
        "src/glad.c",
    }

	externalincludedirs
	{
        "include"
	}

    -- macOS-specific configuration
    filter "system:macosx"

        -- macOS system version and runtime settings
        systemversion "latest"
        staticruntime "On"
        
        -- Set Position-Independent Code flag (important for shared libraries or macOS in general)
        pic "On"

    -- Windows-specific configuration
    filter "system:windows"
        systemversion "latest"
        staticruntime "On"
        
    -- Configuration-specific settings (Debug and Release)
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
