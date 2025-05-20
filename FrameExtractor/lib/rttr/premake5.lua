project "RTTR"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    rtti "On"
    includedirs {
        "src",
        "src/rttr",
        "src/rttr/detail"
    }

    files {
        "src/rttr/**.h",
        "src/rttr/**.cpp"
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
        defines { "RTTR_DEBUG" }
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        defines { "RTTR_RELEASE" }
        optimize "on"

    filter "configurations:Distribution"
        runtime "Release"
        defines { "RTTR_RELEASE" }
        optimize "on"

