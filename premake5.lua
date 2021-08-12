workspace "Logger"
    architecture "x64"

    configurations {
        "Test",
        "SingleIncludeTest"
    }

    startproject "Logger"

-- This is a helper variable, to concatenate the sys-arch
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Logger"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    targetsuffix "_Test"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    outputPath = "bin/" .. outputdir .. "/%{prj.name}/"
    debugdir(outputPath)

	files {
        "logger/**.hpp",
        "logger/**.cpp",
        "logger/**.h"
    }

    includedirs {
        "logger"
    }

    filter { "configurations:SingleIncludeTest" }
        defines { "_DEBUG", "SINGLE_INCLUDE_TEST" }
        symbols "On"
        removefiles { "logger/**" }
        includedirs { "single_include" }
        files { "logger/main.cpp" }

    filter { "configurations:Test" }
        defines { "_DEBUG", "GABE_LOGGER_IMPL" }
        symbols "On"

    filter { "system:Unix", "system:Mac" }
        systemversion "latest"

	filter "system:Windows"
        systemversion "latest"

