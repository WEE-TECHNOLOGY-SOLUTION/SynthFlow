cmake_minimum_required(VERSION 3.14)

# ------------------------------------------------------------------------------
# Project Configuration
# ------------------------------------------------------------------------------
project(SynthFlow
    VERSION 0.0.30
    DESCRIPTION "SynthFlow Programming Language Compiler"
    HOMEPAGE_URL "https://synthflow.dev"
    LANGUAGES CXX
)

# ------------------------------------------------------------------------------
# C++ Standard
# ------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------------------
# Build Type
# ------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# ------------------------------------------------------------------------------
# Options
# ------------------------------------------------------------------------------
option(SYNTHFLOW_BUILD_SHARED "Build shared libraries" OFF)
option(SYNTHFLOW_BUILD_TESTS "Build unit tests" ON)
option(SYNTHFLOW_ENABLE_LTO "Enable Link Time Optimization" OFF)
option(SYNTHFLOW_STATIC_RUNTIME "Use static runtime libraries" ON)

# Cross-compilation options
option(SYNTHFLOW_CROSS_COMPILE "Enable cross-compilation mode" OFF)
set(SYNTHFLOW_TARGET_ARCH "" CACHE STRING "Target architecture for cross-compilation")

# ------------------------------------------------------------------------------
# Platform Detection
# ------------------------------------------------------------------------------
if(WIN32)
    set(SYNTHFLOW_PLATFORM "windows")
elseif(APPLE)
    set(SYNTHFLOW_PLATFORM "macos")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(SYNTHFLOW_PLATFORM "linux")
else()
    set(SYNTHFLOW_PLATFORM "unknown")
    message(WARNING "Unknown platform: ${CMAKE_SYSTEM_NAME}")
endif()

message(STATUS "SynthFlow Build Configuration:")
message(STATUS "  Platform: ${SYNTHFLOW_PLATFORM}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Architecture: ${CMAKE_SYSTEM_PROCESSOR}")

# ------------------------------------------------------------------------------
# Cross-Compilation Configuration
# ------------------------------------------------------------------------------
if(SYNTHFLOW_CROSS_COMPILE)
    message(STATUS "Cross-compilation enabled for: ${SYNTHFLOW_TARGET_ARCH}")

    if(SYNTHFLOW_TARGET_ARCH STREQUAL "windows-x64" AND NOT WIN32)
        set(CMAKE_SYSTEM_NAME Windows)
        set(CMAKE_SYSTEM_PROCESSOR x86_64)
        set(TOOLCHAIN_PREFIX "x86_64-w64-mingw32-")
        set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
        set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
        set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}windres)

    elseif(SYNTHFLOW_TARGET_ARCH STREQUAL "linux-arm64" AND NOT CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
        set(CMAKE_SYSTEM_NAME Linux)
        set(CMAKE_SYSTEM_PROCESSOR aarch64)
        set(TOOLCHAIN_PREFIX "aarch64-linux-gnu-")
        set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
        set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)

    elseif(SYNTHFLOW_TARGET_ARCH STREQUAL "macos-arm64" AND NOT APPLE)
        message(WARNING "Cross-compiling for macOS from non-macOS is not fully supported")
    endif()
endif()

# ------------------------------------------------------------------------------
# Include Platform-Specific Modules
# ------------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Include platform configuration
if(WIN32)
    include(Windows)
elseif(APPLE)
    include(MacOS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include(Linux)
endif()

# ------------------------------------------------------------------------------
# Global Compiler Flags
# ------------------------------------------------------------------------------
if(MSVC)
    # MSVC compiler flags
    add_compile_options(/W4 /utf-8)

    # Static runtime linking for portable binaries
    if(SYNTHFLOW_STATIC_RUNTIME)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()

    # Whole program optimization in release
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/GL)
        add_link_options(/LTCG)
    endif()

    # Define Windows version
    add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX _WIN32_WINNT=0x0601)

else()
    # GCC/Clang compiler flags
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)

    # Position independent code for shared libraries
    if(SYNTHFLOW_BUILD_SHARED)
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    endif()

    # Link time optimization
    if(SYNTHFLOW_ENABLE_LTO AND CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-flto)
        add_link_options(-flto)
    endif()

    # Debug symbols
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g3)
    endif()
endif()

# ------------------------------------------------------------------------------
# Output Directories
# ------------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# ------------------------------------------------------------------------------
# External Dependencies
# ------------------------------------------------------------------------------

# CLI11 - Command line parser (header-only)
include(FetchContent)
FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG v2.4.1
)
FetchContent_MakeAvailable(cli11)

# Find libcurl using our custom module
find_package(Libcurl QUIET)
if(Libcurl_FOUND)
    message(STATUS "libcurl found: ${Libcurl_VERSION}")
else()
    message(STATUS "libcurl not found - HTTP features will use platform defaults")
endif()

# ------------------------------------------------------------------------------
# Source Directories
# ------------------------------------------------------------------------------
include_directories(compiler/include)

# ------------------------------------------------------------------------------
# Modular Libraries
# ------------------------------------------------------------------------------

# Lexer
add_library(lexer compiler/src/lexer/lexer.cpp)

# AST
add_library(ast compiler/src/ast/ast_visitor.cpp)

# Parser
add_library(parser compiler/src/parser/parser.cpp)
target_link_libraries(parser ast lexer)

# Semantic
add_library(semantic compiler/src/semantic/semantic_analyzer.cpp)
target_link_libraries(semantic ast)

# Codegen (Renamed to synthflow_codegen to avoid policy CMP0171)
add_library(synthflow_codegen compiler/src/codegen/code_generator.cpp)
target_link_libraries(synthflow_codegen ast semantic)

# HTTP Client
add_library(http_client compiler/src/http/http_client.cpp)
if(WIN32)
    target_link_libraries(http_client winhttp ws2_32 shell32)
elseif(APPLE)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    find_library(SECURITY_FRAMEWORK Security)
    target_link_libraries(http_client ${COREFOUNDATION_FRAMEWORK} ${SECURITY_FRAMEWORK})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(http_client pthread dl)
endif()

# HTTP Server
add_library(http_server compiler/src/http/http_server.cpp)
if(WIN32)
    target_link_libraries(http_server ws2_32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(http_server pthread)
endif()

# Interpreter (depends on http_server for web framework)
add_library(interpreter compiler/src/interpreter/interpreter.cpp)
target_link_libraries(interpreter ast http_client http_server)

# JavaScript Transpiler
add_library(js_transpiler compiler/src/codegen/js_transpiler.cpp)
target_link_libraries(js_transpiler ast)

# WebAssembly Transpiler
add_library(wasm_transpiler compiler/src/codegen/wasm_transpiler.cpp)
target_link_libraries(wasm_transpiler ast)

# Apply platform-specific settings to all libraries
foreach(lib lexer ast parser semantic synthflow_codegen http_client http_server interpreter js_transpiler wasm_transpiler)
    if(WIN32)
        synthflow_apply_windows_settings(${lib})
    elseif(APPLE)
        synthflow_apply_macos_settings(${lib})
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        synthflow_apply_linux_settings(${lib})
    endif()
endforeach()

# ------------------------------------------------------------------------------
# Executables
# ------------------------------------------------------------------------------

# Main Compiler Executable
add_executable(synthflow compiler/src/main.cpp)
target_link_libraries(synthflow
    parser
    semantic
    synthflow_codegen
    interpreter
    js_transpiler
    wasm_transpiler
    ast
    lexer
    http_client
    http_server
    CLI11::CLI11
)

# Apply platform-specific settings
if(WIN32)
    synthflow_apply_windows_settings(synthflow)
elseif(APPLE)
    synthflow_apply_macos_settings(synthflow)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    synthflow_apply_linux_settings(synthflow)
endif()

# LSP Server
add_executable(synthflow-lsp lsp-server/src/main.cpp)
target_link_libraries(synthflow-lsp parser semantic ast lexer)

if(WIN32)
    synthflow_apply_windows_settings(synthflow-lsp)
elseif(APPLE)
    synthflow_apply_macos_settings(synthflow-lsp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    synthflow_apply_linux_settings(synthflow-lsp)
endif()

# MCP Server
add_executable(synthflow-mcp mcp-server/src/main.cpp)
target_link_libraries(synthflow-mcp parser semantic interpreter ast lexer http_server)

if(WIN32)
    synthflow_apply_windows_settings(synthflow-mcp)
elseif(APPLE)
    synthflow_apply_macos_settings(synthflow-mcp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    synthflow_apply_linux_settings(synthflow-mcp)
endif()

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------
if(SYNTHFLOW_BUILD_TESTS)
    enable_testing()
    # Add test directory if it exists with CMakeLists.txt
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt")
        add_subdirectory(tests)
    endif()
endif()

# ------------------------------------------------------------------------------
# Installation
# ------------------------------------------------------------------------------
install(TARGETS synthflow synthflow-lsp synthflow-mcp
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(TARGETS lexer parser ast semantic synthflow_codegen interpreter http_client http_server js_transpiler wasm_transpiler
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY compiler/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Install standard library
if(EXISTS "${CMAKE_SOURCE_DIR}/stdlib")
    install(DIRECTORY stdlib/
        DESTINATION share/synthflow/stdlib
        FILES_MATCHING PATTERN "*.sf"
    )
endif()

# Install documentation
if(EXISTS "${CMAKE_SOURCE_DIR}/docs")
    install(DIRECTORY docs/
        DESTINATION share/doc/synthflow
        OPTIONAL
    )
endif()

# Install man pages
if(EXISTS "${CMAKE_SOURCE_DIR}/docs/man")
    install(DIRECTORY docs/man/
        DESTINATION share/man
        FILES_MATCHING PATTERN "*.1" PATTERN "*.3"
        OPTIONAL
    )
endif()

# ------------------------------------------------------------------------------
# Packaging Configuration (CPack)
# ------------------------------------------------------------------------------
set(CPACK_PACKAGE_NAME "SynthFlow")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SynthFlow Programming Language Compiler")
set(CPACK_PACKAGE_DESCRIPTION "SynthFlow is a modern programming language designed for building AI-powered applications.")
set(CPACK_PACKAGE_VENDOR "SynthFlow")
set(CPACK_PACKAGE_CONTACT "support@synthflow.dev")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://synthflow.dev")

# Set package file name based on platform
if(WIN32)
    set(CPACK_GENERATOR "ZIP;WIX;NSIS")
    set(CPACK_PACKAGE_FILE_NAME "synthflow-${PROJECT_VERSION}-windows-x64")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ;DragNDrop;productbuild")
    set(CPACK_PACKAGE_FILE_NAME "synthflow-${PROJECT_VERSION}-macos-universal")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CPACK_GENERATOR "TGZ;DEB;RPM")
    set(CPACK_PACKAGE_FILE_NAME "synthflow-${PROJECT_VERSION}-linux-${CMAKE_SYSTEM_PROCESSOR}")
endif()

# Common CPack settings
if(EXISTS "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/README.md")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
endif()

# Include CPack
include(CPack)

# ------------------------------------------------------------------------------
# Print Platform-Specific Configuration
# ------------------------------------------------------------------------------
message(STATUS "")
message(STATUS "SynthFlow Build Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Platform: ${SYNTHFLOW_PLATFORM}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")

if(WIN32)
    synthflow_print_windows_config()
elseif(APPLE)
    synthflow_print_macos_config()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    synthflow_print_linux_config()
endif()

message(STATUS "")
message(STATUS "Build targets:")
message(STATUS "  synthflow      - Main compiler")
message(STATUS "  synthflow-lsp  - Language Server Protocol server")
message(STATUS "  synthflow-mcp  - Model Context Protocol server")
message(STATUS "")