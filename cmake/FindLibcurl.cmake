# ------------------------------------------------------------------------------
# FindLibcurl.cmake - CMake module to find libcurl on all platforms
# ------------------------------------------------------------------------------
# This module finds the curl library on Windows, macOS, and Linux.
# It handles both system-installed curl and vcpkg/conan-provided curl.
#
# Usage:
#   find_package(Libcurl REQUIRED)
#   target_link_libraries(your_target PRIVATE Libcurl::Libcurl)
#
# This module defines:
#   Libcurl_FOUND        - True if libcurl was found
#   Libcurl_INCLUDE_DIRS - Include directories for libcurl
#   Libcurl_LIBRARIES    - Libraries to link against
#   Libcurl_VERSION      - Version of libcurl found
#   Libcurl::Libcurl     - Imported target for libcurl
# ------------------------------------------------------------------------------

include(FindPackageHandleStandardArgs)

# ------------------------------------------------------------------------------
# Try to find curl using CMake's built-in FindCURL first
# ------------------------------------------------------------------------------
find_package(CURL QUIET)

if(CURL_FOUND)
    # Create imported target if not already defined
    if(NOT TARGET Libcurl::Libcurl)
        add_library(Libcurl::Libcurl UNKNOWN IMPORTED)
        set_target_properties(Libcurl::Libcurl PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CURL_INCLUDE_DIRS}"
            IMPORTED_LOCATION "${CURL_LIBRARIES}"
        )
    endif()

    set(Libcurl_FOUND TRUE)
    set(Libcurl_INCLUDE_DIRS ${CURL_INCLUDE_DIRS})
    set(Libcurl_LIBRARIES ${CURL_LIBRARIES})
    set(Libcurl_VERSION ${CURL_VERSION_STRING})

    # Check for SSL support
    include(CheckSymbolExists)
    set(CMAKE_REQUIRED_INCLUDES ${CURL_INCLUDE_DIRS})
    check_symbol_exists(CURL_VERSION_SSL "curl/curl.h" LIBCURL_HAS_SSL)

    return()
endif()

# ------------------------------------------------------------------------------
# Manual search for libcurl if CMake's FindCURL didn't work
# ------------------------------------------------------------------------------

# Possible include directories
set(_LIBCURL_INCLUDE_SEARCH_PATHS
    # Standard system locations
    /usr/include
    /usr/local/include
    /opt/local/include
    /opt/curl/include

    # macOS Homebrew
    /usr/local/opt/curl/include
    /opt/homebrew/opt/curl/include
    /opt/homebrew/include

    # Linux standard locations
    /usr/include/x86_64-linux-gnu

    # Windows specific
    $ENV{CURL_ROOT}/include
    $ENV{CURL_DIR}/include
    "C:/curl/include"
    "C:/Program Files/curl/include"
    "C:/Program Files (x86)/curl/include"

    # vcpkg locations
    $ENV{VCPKG_ROOT}/installed/x64-windows/include
    $ENV{VCPKG_ROOT}/installed/x64-linux/include
    $ENV{VCPKG_ROOT}/installed/x64-osx/include
    $ENV{VCPKG_ROOT}/installed/arm64-osx/include

    # Conan locations
    $ENV{CONAN_ROOT}/include
)

# Possible library directories
set(_LIBCURL_LIBRARY_SEARCH_PATHS
    # Standard system locations
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /opt/curl/lib

    # macOS Homebrew
    /usr/local/opt/curl/lib
    /opt/homebrew/opt/curl/lib
    /opt/homebrew/lib

    # Linux standard locations
    /usr/lib/x86_64-linux-gnu
    /usr/lib/aarch64-linux-gnu

    # Windows specific
    $ENV{CURL_ROOT}/lib
    $ENV{CURL_DIR}/lib
    "C:/curl/lib"
    "C:/Program Files/curl/lib"
    "C:/Program Files (x86)/curl/lib"

    # vcpkg locations
    $ENV{VCPKG_ROOT}/installed/x64-windows/lib
    $ENV{VCPKG_ROOT}/installed/x64-linux/lib
    $ENV{VCPKG_ROOT}/installed/x64-osx/lib
    $ENV{VCPKG_ROOT}/installed/arm64-osx/lib

    # Conan locations
    $ENV{CONAN_ROOT}/lib
)

# ------------------------------------------------------------------------------
# Find include directory
# ------------------------------------------------------------------------------
find_path(Libcurl_INCLUDE_DIR
    NAMES curl/curl.h
    PATHS ${_LIBCURL_INCLUDE_SEARCH_PATHS}
    PATH_SUFFIXES curl
)

# ------------------------------------------------------------------------------
# Find library
# ------------------------------------------------------------------------------
# Library names to search for
if(WIN32)
    set(_LIBCURL_NAMES libcurl curl libcurl_a curl_a)
else()
    set(_LIBCURL_NAMES curl)
endif()

# Search for the library
find_library(Libcurl_LIBRARY
    NAMES ${_LIBCURL_NAMES}
    PATHS ${_LIBCURL_LIBRARY_SEARCH_PATHS}
    PATH_SUFFIXES curl
)

# On Windows, also look for the DLL
if(WIN32)
    find_file(Libcurl_DLL
        NAMES libcurl.dll curl.dll
        PATHS ${_LIBCURL_LIBRARY_SEARCH_PATHS}
        PATH_SUFFIXES bin
        DOC "Path to libcurl DLL"
    )
endif()

# ------------------------------------------------------------------------------
# Determine the version
# ------------------------------------------------------------------------------
if(Libcurl_INCLUDE_DIR AND EXISTS "${Libcurl_INCLUDE_DIR}/curl/curl.h")
    # Extract version from curl/curl.h
    file(READ "${Libcurl_INCLUDE_DIR}/curl/curl.h" _LIBCURL_HEADER_CONTENT)

    string(REGEX MATCH "#define[ \t]+LIBCURL_VERSION[ \t]+\"([0-9]+\\.[0-9]+\\.[0-9]+)\""
           _LIBCURL_VERSION_MATCH "${_LIBCURL_HEADER_CONTENT}")

    if(_LIBCURL_VERSION_MATCH)
        set(Libcurl_VERSION "${CMAKE_MATCH_1}")
    else()
        string(REGEX MATCH "#define[ \t]+LIBCURL_VERSION_NUM[ \t]+0x([0-9a-fA-F]+)"
               _LIBCURL_VERSION_NUM_MATCH "${_LIBCURL_HEADER_CONTENT}")
        if(_LIBCURL_VERSION_NUM_MATCH)
            # Convert hex version to decimal
            string(REGEX MATCH "([0-9a-fA-F][0-9a-fA-F])([0-9a-fA-F][0-9a-fA-F])([0-9a-fA-F][0-9a-fA-F])"
                   _LIBCURL_VERSION_PARTS "${CMAKE_MATCH_1}")
            math(EXPR LIBCURL_MAJOR "0x${CMAKE_MATCH_1}")
            math(EXPR LIBCURL_MINOR "0x${CMAKE_MATCH_2}")
            math(EXPR LIBCURL_PATCH "0x${CMAKE_MATCH_3}")
            set(Libcurl_VERSION "${LIBCURL_MAJOR}.${LIBCURL_MINOR}.${LIBCURL_PATCH}")
        endif()
    endif()
endif()

# ------------------------------------------------------------------------------
# Handle standard arguments
# ------------------------------------------------------------------------------
find_package_handle_standard_args(Libcurl
    REQUIRED_VARS Libcurl_LIBRARY Libcurl_INCLUDE_DIR
    VERSION_VAR Libcurl_VERSION
)

# ------------------------------------------------------------------------------
# Create imported target
# ------------------------------------------------------------------------------
if(Libcurl_FOUND AND NOT TARGET Libcurl::Libcurl)
    add_library(Libcurl::Libcurl UNKNOWN IMPORTED)

    set_target_properties(Libcurl::Libcurl PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Libcurl_INCLUDE_DIR}"
        IMPORTED_LOCATION "${Libcurl_LIBRARY}"
    )

    # On Windows, link additional libraries for curl
    if(WIN32)
        set_property(TARGET Libcurl::Libcurl APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES ws2_32 crypt32 winmm)
    endif()

    # On Linux, link with OpenSSL if available
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        find_package(OpenSSL QUIET)
        if(OpenSSL_FOUND)
            set_property(TARGET Libcurl::Libcurl APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES OpenSSL::SSL OpenSSL::Crypto)
        endif()
    endif()

    # On macOS, link with Security framework
    if(APPLE)
        find_library(SECURITY_FRAMEWORK Security)
        if(SECURITY_FRAMEWORK)
            set_property(TARGET Libcurl::Libcurl APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES ${SECURITY_FRAMEWORK})
        endif()
    endif()
endif()

# ------------------------------------------------------------------------------
# Set variables for compatibility
# ------------------------------------------------------------------------------
if(Libcurl_FOUND)
    set(Libcurl_INCLUDE_DIRS ${Libcurl_INCLUDE_DIR})
    set(Libcurl_LIBRARIES ${Libcurl_LIBRARY})

    mark_as_advanced(Libcurl_INCLUDE_DIR Libcurl_LIBRARY Libcurl_DLL)
endif()

# ------------------------------------------------------------------------------
# Print status
# ------------------------------------------------------------------------------
if(Libcurl_FOUND)
    message(STATUS "Found libcurl: ${Libcurl_LIBRARY} (version ${Libcurl_VERSION})")
else()
    message(STATUS "libcurl not found. HTTP client features may be limited.")
endif()