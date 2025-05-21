project "OpenXLSX"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "On"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    includedirs {
        "inc",
        "inc/headers",
        "inc/external/pugixml",
        "inc/external/nowide",
        "inc/external/zippy"
    }

    files {
        "sources/**.cpp"
    }


    -- macOS-specific configuration
    filter "system:macosx"
        systemversion "latest"
        pic "On"

    -- Windows-specific configuration
    filter "system:windows"
        systemversion "latest"
   
    -- Configuration-specific settings (Debug and Release)
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:Distribution"
        runtime "Release"
        optimize "on"

