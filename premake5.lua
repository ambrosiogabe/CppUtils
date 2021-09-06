workspace "CppUtils"
    architecture "x64"

    configurations {
        "Test"
    }

    startproject "CppUtils"

-- This is a helper variable, to concatenate the sys-arch
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "CppUtils"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    outputPath = "bin/" .. outputdir .. "/%{prj.name}/"
    debugdir(outputPath)

	files {
        "single_include/cppUtils/cppUtils.hpp"
    }

    filter { "configurations:Test" }
        defines { "_DEBUG", "GABE_CPP_UTILS_TEST_MAIN", "GABE_CPP_UTILS_IMPL" }
        symbols "On"

    filter { "system:Unix", "system:Mac" }
        systemversion "latest"

	filter "system:Windows"
        systemversion "latest"

