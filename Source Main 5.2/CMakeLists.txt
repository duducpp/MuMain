cmake_minimum_required(VERSION 3.20)

project(Main LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)


# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CONFIGURATION})

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CMAKE_GENERATOR_PLATFORM "Win32" CACHE STRING "" FORCE)

    set(CMAKE_VS_PLATFORM_NAME "Win32")

    set_property(TARGET ${PROJECT_NAME} PROPERTY
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    )
endif()

# Collect all source files
file(GLOB_RECURSE SOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.rc"
)

# Separate precompiled header source
set(PCH_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/source/StdAfx.cpp")
set(PCH_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/source/stdafx.h")

# Remove PCH source from main sources
list(REMOVE_ITEM SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${PCH_SOURCE}")

# Create executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES} ${PCH_SOURCE})

# Set precompiled header
target_precompile_headers(${PROJECT_NAME} PRIVATE "${PCH_HEADER}")

if(MSVC)
# Organize source files into folders in Visual Studio using source_group
# Group files by their relative directory structure
foreach(SOURCE_FILE ${SOURCE_FILES} ${PCH_SOURCE})
    if(IS_ABSOLUTE ${SOURCE_FILE})
        set(ABS_SOURCE_FILE ${SOURCE_FILE})
    else()
        set(ABS_SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE})
    endif()
    
    # Get the directory part of the file path
    get_filename_component(FILE_DIR ${ABS_SOURCE_FILE} DIRECTORY)
    # Get relative path from project root
    file(RELATIVE_PATH RELATIVE_DIR ${CMAKE_CURRENT_SOURCE_DIR} ${FILE_DIR})
    
    # Extract the part after "source/" for the filter path
    string(REGEX REPLACE "^source[/\\\\]" "" FILTER_PATH ${RELATIVE_DIR})
    
    # Check if file is in root source directory (RELATIVE_DIR is "source" or FILTER_PATH is empty)
    if(RELATIVE_DIR STREQUAL "source" OR FILTER_PATH STREQUAL "")
        # Files in root source directory
        source_group("Source Files" FILES ${SOURCE_FILE})
    else()
        # Convert forward slashes to backslashes for Visual Studio filter format
        string(REPLACE "/" "\\" FILTER_PATH ${FILTER_PATH})
        # Group by directory structure
        source_group("${FILTER_PATH}" FILES ${SOURCE_FILE})
    endif()
endforeach()
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    UNICODE
    _UNICODE
)
if(MSVC)
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:/GR> # RTTI for Debug
    $<$<CONFIG:Release>:
        /O2      # Maximum optimization
        /Ob2     # Inline function expansion
        /Oi      # Generate intrinsic functions
        /Ot      # Favor fast code
        /Oy      # Frame pointer omission
        /GF      # Enable string pooling
        /GS      # Buffer security check
    >
    /Zi          # Generate PDB file
    /MP          # Multi-processor compilation
    /Zc:wchar_t  # Treat wchar_t as native type
)

# /MT for Release, /MTd for Debug
set_property(TARGET ${PROJECT_NAME} PROPERTY
    MSVC_RUNTIME_LIBRARY
    "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)
endif()

target_link_options(${PROJECT_NAME} PRIVATE
    /DEBUG                                  # Generate PDB file
    /NODEFAULTLIB:libc                      # Ignore standard C libraries
    $<$<CONFIG:Debug>:/NODEFAULTLIB:libcmt> # Ignore standard C++ libraries for Debug
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/netcore/includes
)

# Link directories
target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/netcore/lib
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    crypt32
    dsound
    dxguid
    glu32
    imm32
    opengl32
    shlwapi
    turbojpeg-static
    urlmon
    version
    vfw32
    wininet
    winmm
    ws2_32
    wzAudio
)

# MinGW (32-bit or 64-bit) cross-toolchains: link against the Windows/OpenGL
# system libraries so that gl*, wgl*, Imm*, timeGetTime, etc. resolve.
# For turbojpeg and wzAudio we link against the logically named libraries,
# which must be provided as MinGW-compatible .a import libraries.
if (CMAKE_CXX_COMPILER MATCHES "mingw32" OR CMAKE_CXX_COMPILER MATCHES "w64-mingw32")
    target_link_directories(Main PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib"
    )

    set(MU_TURBOJPEG_STATIC_LIB "" CACHE FILEPATH "Path to MinGW-w64 static libturbojpeg.a")

    if (MU_TURBOJPEG_STATIC_LIB STREQUAL "" OR MU_TURBOJPEG_STATIC_LIB MATCHES "-NOTFOUND$")
        unset(MU_TURBOJPEG_STATIC_LIB CACHE)
    endif()

    if (NOT MU_TURBOJPEG_STATIC_LIB)
        find_library(MU_TURBOJPEG_STATIC_LIB
            NAMES turbojpeg
            PATHS
            "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib"
            NO_CMAKE_FIND_ROOT_PATH
        )
    endif()

    if (NOT MU_TURBOJPEG_STATIC_LIB)
        set(_MU_OLD_FIND_LIBRARY_SUFFIXES "${CMAKE_FIND_LIBRARY_SUFFIXES}")
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
        set(_MU_TURBOJPEG_SEARCH_PATHS "")

        foreach(_mu_root IN LISTS CMAKE_FIND_ROOT_PATH)
            list(APPEND _MU_TURBOJPEG_SEARCH_PATHS "${_mu_root}/lib" "${_mu_root}/usr/lib")
        endforeach()

        list(APPEND _MU_TURBOJPEG_SEARCH_PATHS "/usr/i686-w64-mingw32/lib" "/usr/x86_64-w64-mingw32/lib")
        find_library(MU_TURBOJPEG_STATIC_LIB
            NAMES turbojpeg
            PATHS ${_MU_TURBOJPEG_SEARCH_PATHS}
            NO_CMAKE_FIND_ROOT_PATH
        )
        unset(_MU_TURBOJPEG_SEARCH_PATHS)
        unset(_mu_root)
        set(CMAKE_FIND_LIBRARY_SUFFIXES "${_MU_OLD_FIND_LIBRARY_SUFFIXES}")
        unset(_MU_OLD_FIND_LIBRARY_SUFFIXES)
    endif()

    if (NOT MU_TURBOJPEG_STATIC_LIB OR MU_TURBOJPEG_STATIC_LIB MATCHES "\\.dll\\.a$")
        message(FATAL_ERROR "Static libturbojpeg not found (libturbojpeg.a) or CMake picked an import library (.dll.a): '${MU_TURBOJPEG_STATIC_LIB}'. Install/build a MinGW-w64 static libjpeg-turbo to avoid shipping libturbojpeg.dll, or set MU_TURBOJPEG_STATIC_LIB to the full path of libturbojpeg.a.")
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${MU_TURBOJPEG_STATIC_LIB}
    )
endif()

