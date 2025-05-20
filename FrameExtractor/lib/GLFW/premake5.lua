project "GLFW"
    kind "StaticLib"
    language "C"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    -- Common GLFW source files
    files
    {

        "src/context.c",
        "src/init.c",
        "src/input.c",
        "src/monitor.c",
        "src/vulkan.c",
        "src/window.c",
		"src/egl_context.c",
		"src/osmesa_context.c",
		"src/platform.c",
		"src/null_init.c",
		"src/null_monitor.c",
		"src/null_window.c",
		"src/null_joystick.c"
    }

    -- macOS-specific configuration
    filter "system:macosx"
        -- Enable Objective-C language for macOS-specific files
        language "C"
		compileas "Objective-C"

        -- macOS-specific source files
        files
        {
            "src/cocoa_init.m",
            "src/cocoa_joystick.m",
            "src/cocoa_monitor.m",
			"src/cocoa_time.h",
            "src/cocoa_time.c",
            "src/cocoa_window.m",
			"src/posix_module.c",
			"src/posix_thread.h",
            "src/posix_thread.c",
            "src/nsgl_context.m"
        }

        -- Define the _GLFW_COCOA macro for Cocoa-specific code
        defines
        {
            "_GLFW_COCOA"
        }

        -- Link against macOS frameworks
        links
        {
            "Cocoa.framework",           -- Cocoa for Objective-C-based windowing
            "IOKit.framework",           -- For input devices (joysticks, keyboard, etc.)
			"QuartzCore.framework",  -- For common utilities (e.g., strings, file operations)
            "CoreFoundation.framework",  -- For common utilities (e.g., strings, file operations)
		}

        -- macOS system version and runtime settings
        systemversion "latest"
        staticruntime "On"
        
        -- Set Position-Independent Code flag (important for shared libraries or macOS in general)
        pic "On"

    -- Windows-specific configuration
    filter "system:windows"
        systemversion "latest"
        staticruntime "On"
        
        -- Windows-specific source files
        files
        {
            "src/win32_init.c",
            "src/win32_joystick.c",
            "src/win32_monitor.c",
            "src/win32_time.c",
            "src/win32_thread.c",
            "src/win32_window.c",
            "src/win32_module.c",
			"src/wgl_context.c"
        }

        -- Define the _GLFW_WIN32 macro for Windows-specific code
        defines 
        { 
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }

    -- Configuration-specific settings (Debug and Release)
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
