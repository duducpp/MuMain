cmake_minimum_required(VERSION 3.20)

project(MUnique.Client.Library
    VERSION 1.0.0
    LANGUAGES CSharp
    DESCRIPTION "MUnique Client Library"
)

# Target framework and platform
set(RUNTIME_IDENTIFIER "win-x86")

# Set default configuration if not using multi-config generator
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    endif()
    set(CONFIGURATION ${CMAKE_BUILD_TYPE})
else()
    # For multi-config generators (Visual Studio), use generator expression
    set(CONFIGURATION $<CONFIG>)
endif()

# Custom target for publishing with PublishSingleFile
add_custom_target(${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E echo "Publishing ${PROJECT_NAME} for configuration: $<CONFIG>..."
    COMMAND dotnet publish
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.csproj"
        --configuration $<CONFIG>
        --runtime ${RUNTIME_IDENTIFIER}
        --self-contained true
        -p:PublishSingleFile=false
        -p:EnableCompressionInSingleFile=true
        -p:DebugSymbols=true
        -p:DebugType=portable
        -p:Optimize=$<IF:$<STREQUAL:$<CONFIG>,Release>,true,false>
        --output "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Publishing ${PROJECT_NAME} as single file for ${RUNTIME_IDENTIFIER} ($<CONFIG>)"
    VERBATIM
)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR}
    PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME}
)