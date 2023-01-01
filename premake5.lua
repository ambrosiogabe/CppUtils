workspace "CppUtils"
    architecture "x64"

    configurations {
        "Test"
    }

    startproject "CppUtils"

-- This is a helper variable, to concatenate the sys-arch
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "CppUtilsTest_C"
    kind "ConsoleApp"
    staticruntime "on"
    language "C"
    cdialect "C11"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    outputPath = "bin/" .. outputdir .. "/%{prj.name}/"
    debugdir(outputPath)

    buildoptions { 
        "-std=c11" 
    }

    files {
        "single_include/cppUtils/cppUtils.h",
        "single_include/cppUtils/**.c"
    }

    includedirs {
        "single_include"
    }

    defines { "_DEBUG", "GABE_CPP_UTILS_TEST_MAIN", "GABE_CPP_UTILS_IMPL" }
    symbols "On"

    filter { "system:Unix", "system:Mac" }
        systemversion "latest"

    filter "system:Windows"
        systemversion "latest"
        buildoptions {
            "/W4",
            "/WX"
        }

project "CppUtilsTest_Cpp"
    kind "ConsoleApp"
    staticruntime "on"
    language "C++"
    cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    outputPath = "bin/" .. outputdir .. "/%{prj.name}/"
    debugdir(outputPath)
    
	files {
        "single_include/cppUtils/cppUtils.h",
        "single_include/cppUtils/**.cpp"
    }

    includedirs {
        "single_include"
    }

    defines { "_DEBUG", "GABE_CPP_UTILS_TEST_MAIN", "GABE_CPP_UTILS_IMPL" }
    symbols "On"

    filter { "system:Unix", "system:Mac" }
        systemversion "latest"

	filter "system:Windows"
        systemversion "latest"
        buildoptions {
            "/W4",
            "/WX"
        }

