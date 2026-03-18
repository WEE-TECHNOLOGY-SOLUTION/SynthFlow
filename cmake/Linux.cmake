# ------------------------------------------------------------------------------
# Linux-specific CMake configuration for SynthFlow
# ------------------------------------------------------------------------------
# This module handles Linux-specific build settings including:
# - AppImage generation
# - Debian package (.deb) creation
# - RPM package creation
# - Linux-specific compiler flags and system libraries
# ------------------------------------------------------------------------------

# Prevent multiple inclusion
if(SYNTHFLOW_LINUX_CMAKE_INCLUDED)
    return()
endif()
set(SYNTHFLOW_LINUX_CMAKE_INCLUDED TRUE)

# ------------------------------------------------------------------------------
# Linux-specific compiler flags
# ------------------------------------------------------------------------------
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Position Independent Code for shared libraries
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    # Optimization flags for release builds
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

    # Debug flags
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0 -DDEBUG")

    # Address Sanitizer support (optional)
    option(SYNTHFLOW_ENABLE_ASAN "Enable Address Sanitizer" OFF)
    if(SYNTHFLOW_ENABLE_ASAN)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    endif()

    # Link time optimization for release
    option(SYNTHFLOW_ENABLE_LTO "Enable Link Time Optimization" OFF)
    if(SYNTHFLOW_ENABLE_LTO)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
    endif()
endif()

# ------------------------------------------------------------------------------
# Linux system libraries
# ------------------------------------------------------------------------------
set(SYNTHFLOW_LINUX_LIBS
    pthread     # POSIX threads
    dl          # Dynamic linking
    rt          # Real-time extensions (optional on newer systems)
)

# Check for libcurl
find_package(CURL QUIET)
if(CURL_FOUND)
    list(APPEND SYNTHFLOW_LINUX_LIBS CURL::libcurl)
endif()

# Check for systemd (for service integration)
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(SYSTEMD libsystemd)
endif()

# ------------------------------------------------------------------------------
# AppImage Configuration
# ------------------------------------------------------------------------------
# AppImage creates portable Linux applications that run on any distribution
# Requires: appimagetool or linuxdeploy
# ------------------------------------------------------------------------------

# AppImage output directory
set(SYNTHFLOW_APPIMAGE_DIR "${CMAKE_BINARY_DIR}/AppImage")
set(SYNTHFLOW_APPDIR "${SYNTHFLOW_APPIMAGE_DIR}/AppDir")

# AppImage metadata
set(SYNTHFLOW_APPIMAGE_NAME "SynthFlow-${PROJECT_VERSION}-x86_64.AppImage")

# Desktop file for AppImage
set(SYNTHFLOW_DESKTOP_FILE_CONTENT
"[Desktop Entry]
Version=${PROJECT_VERSION}
Type=Application
Name=SynthFlow
Comment=SynthFlow Programming Language Compiler
Exec=synthflow
Icon=synthflow
Terminal=true
Categories=Development;IDE;Compiler;
Keywords=programming;compiler;language;
StartupNotify=true
")

# Function to create AppImage structure
function(synthflow_prepare_appimage)
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
        return()
    endif()

    # Create AppDir structure
    file(MAKE_DIRECTORY ${SYNTHFLOW_APPDIR}/usr/bin)
    file(MAKE_DIRECTORY ${SYNTHFLOW_APPDIR}/usr/lib)
    file(MAKE_DIRECTORY ${SYNTHFLOW_APPDIR}/usr/share/applications)
    file(MAKE_DIRECTORY ${SYNTHFLOW_APPDIR}/usr/share/icons/hicolor/256x256/apps)
    file(MAKE_DIRECTORY ${SYNTHFLOW_APPDIR}/usr/share/metainfo)

    # Write desktop file
    file(WRITE "${SYNTHFLOW_APPDIR}/usr/share/applications/synthflow.desktop"
         "${SYNTHFLOW_DESKTOP_FILE_CONTENT}")

    # Create symlink for AppImage compatibility
    file(CREATE_LINK "${SYNTHFLOW_APPDIR}/usr/share/applications/synthflow.desktop"
         "${SYNTHFLOW_APPDIR}/synthflow.desktop" SYMBOLIC)
endfunction()

# Function to build AppImage
function(synthflow_build_appimage target)
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
        return()
    endif()

    find_program(LINUXDEPLOY linuxdeploy)
    find_program(APPIMAGETOOL appimagetool)

    if(LINUXDEPLOY)
        add_custom_target(appimage
            COMMAND ${CMAKE_COMMAND} -E make_directory ${SYNTHFLOW_APPDIR}/usr/bin
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${SYNTHFLOW_APPDIR}/usr/bin/
            COMMAND ${LINUXDEPLOY}
                --appdir ${SYNTHFLOW_APPDIR}
                --executable $<TARGET_FILE:${target}>
                --output appimage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Building AppImage..."
        )
    elseif(APPIMAGETOOL)
        add_custom_target(appimage
            COMMAND ${CMAKE_COMMAND} -E make_directory ${SYNTHFLOW_APPDIR}/usr/bin
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${SYNTHFLOW_APPDIR}/usr/bin/
            COMMAND ${APPIMAGETOOL} ${SYNTHFLOW_APPDIR} ${SYNTHFLOW_APPIMAGE_NAME}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Building AppImage..."
        )
    else()
        message(STATUS "linuxdeploy or appimagetool not found. AppImage target disabled.")
    endif()
endfunction()

# ------------------------------------------------------------------------------
# Debian Package Configuration
# ------------------------------------------------------------------------------
set(CPACK_DEBIAN_PACKAGE_NAME "synthflow")
set(CPACK_DEBIAN_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "SynthFlow Team <support@synthflow.dev>")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "SynthFlow Programming Language Compiler

 SynthFlow is a modern programming language designed for building
 AI-powered applications with intuitive syntax and powerful features.")

set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://synthflow.dev")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.17), libstdc++6 (>= 9)")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "synthflow-stdlib")
set(CPACK_DEBIAN_PACKAGE_SUGGESTS "synthflow-lsp, synthflow-docs")

# Control file extras
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    "${CMAKE_SOURCE_DIR}/cmake/debian/postinst"
    "${CMAKE_SOURCE_DIR}/cmake/debian/prerm"
)

# Debian package naming
set(CPACK_DEBIAN_FILE_NAME "synthflow_${PROJECT_VERSION}_amd64.deb")

# Package signing (requires gpg)
option(SYNTHFLOW_DEBIAN_SIGN "Sign Debian package" OFF)
if(SYNTHFLOW_DEBIAN_SIGN)
    set(CPACK_DEBIAN_PACKAGE_SIGN_GPG ON)
endif()

# ------------------------------------------------------------------------------
# RPM Package Configuration
# ------------------------------------------------------------------------------
set(CPACK_RPM_PACKAGE_NAME "synthflow")
set(CPACK_RPM_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_SUMMARY "SynthFlow Programming Language Compiler")
set(CPACK_RPM_PACKAGE_DESCRIPTION
"SynthFlow is a modern programming language designed for building
AI-powered applications with intuitive syntax and powerful features.")

set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_VENDOR "SynthFlow")
set(CPACK_RPM_PACKAGER "SynthFlow Team <support@synthflow.dev>")
set(CPACK_RPM_PACKAGE_URL "https://synthflow.dev")
set(CPACK_RPM_PACKAGE_GROUP "Development/Languages")

# RPM dependencies
set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.17, libstdc++ >= 9")
set(CPACK_RPM_PACKAGE_PROVIDES "synthflow = ${PROJECT_VERSION}")

# Pre/post install scripts
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/cmake/rpm/postinst")
set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/cmake/rpm/prerm")

# RPM package naming
set(CPACK_RPM_FILE_NAME "synthflow-${PROJECT_VERSION}-1.x86_64.rpm")

# RPM spec file extras
set(CPACK_RPM_PACKAGE_RELOCATABLE ON)

# ------------------------------------------------------------------------------
# Arch Linux Package (PKGBUILD) Configuration
# ------------------------------------------------------------------------------
set(SYNTHFLOW_PKGBUILD_CONTENT
"# Maintainer: SynthFlow Team <support@synthflow.dev>
pkgname=synthflow
pkgver=${PROJECT_VERSION}
pkgrel=1
pkgdesc='SynthFlow Programming Language Compiler'
arch=('x86_64')
url='https://synthflow.dev'
license=('MIT')
depends=('glibc' 'gcc-libs')
makedepends=('cmake' 'git')

source=('\$pkgname-\$pkgver.tar.gz::https://github.com/synthflow/synthflow/archive/v\$pkgver.tar.gz')
sha256sums=('SKIP')

build() {
    cd \"\$pkgname-\$pkgver\"
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
}

package() {
    cd \"\$pkgname-\$pkgver\"
    cmake --install build --prefix \"\$pkgdir/usr\"
}
")

# Function to generate PKGBUILD
function(synthflow_generate_pkgbuild)
    file(WRITE "${CMAKE_BINARY_DIR}/PKGBUILD" "${SYNTHFLOW_PKGBUILD_CONTENT}")
endfunction()

# ------------------------------------------------------------------------------
# Function to apply Linux-specific settings to a target
# ------------------------------------------------------------------------------
function(synthflow_apply_linux_settings target)
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
        return()
    endif()

    # Link Linux libraries (using plain signature for consistency)
    target_link_libraries(${target} ${SYNTHFLOW_LINUX_LIBS})

    # Set output directory
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )

    # Install targets
    install(TARGETS ${target}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )

    # Install man page if it exists
    if(EXISTS "${CMAKE_SOURCE_DIR}/docs/synthflow.1")
        install(FILES "${CMAKE_SOURCE_DIR}/docs/synthflow.1"
                DESTINATION share/man/man1)
    endif()

    # Install desktop file
    install(FILES "${CMAKE_SOURCE_DIR}/resources/synthflow.desktop"
            DESTINATION share/applications
            OPTIONAL)

    # Install icons
    install(FILES "${CMAKE_SOURCE_DIR}/resources/synthflow.png"
            DESTINATION share/icons/hicolor/256x256/apps
            OPTIONAL)
endfunction()

# ------------------------------------------------------------------------------
# Function to install stdlib and additional files
# ------------------------------------------------------------------------------
function(synthflow_install_stdlib)
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
        return()
    endif()

    # Install standard library
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/stdlib/"
            DESTINATION share/synthflow/stdlib
            FILES_MATCHING PATTERN "*.sf")

    # Install examples
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/examples/"
            DESTINATION share/synthflow/examples
            OPTIONAL)
endfunction()

# ------------------------------------------------------------------------------
# Print Linux configuration summary
# ------------------------------------------------------------------------------
function(synthflow_print_linux_config)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        message(STATUS "Linux Configuration:")
        message(STATUS "  Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
        message(STATUS "  Debian Package: ${CPACK_DEBIAN_FILE_NAME}")
        message(STATUS "  RPM Package: ${CPACK_RPM_FILE_NAME}")
        message(STATUS "  AppImage: ${SYNTHFLOW_APPIMAGE_NAME}")
        if(SYNTHFLOW_ENABLE_ASAN)
            message(STATUS "  Address Sanitizer: Enabled")
        endif()
        if(SYNTHFLOW_ENABLE_LTO)
            message(STATUS "  Link Time Optimization: Enabled")
        endif()
    endif()
endfunction()