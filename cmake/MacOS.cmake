# ------------------------------------------------------------------------------
# macOS-specific CMake configuration for SynthFlow
# ------------------------------------------------------------------------------
# This module handles macOS-specific build settings including:
# - Application bundle (.app) creation
# - DMG installer generation
# - Code signing configuration
# - macOS-specific compiler flags and frameworks
# ------------------------------------------------------------------------------

# Prevent multiple inclusion
if(SYNTHFLOW_MACOS_CMAKE_INCLUDED)
    return()
endif()
set(SYNTHFLOW_MACOS_CMAKE_INCLUDED TRUE)

# ------------------------------------------------------------------------------
# macOS-specific compiler flags
# ------------------------------------------------------------------------------
if(APPLE)
    # Minimum macOS version (10.15 Catalina for C++17 full support)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS version")

    # Architecture support (Universal binaries)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Target architectures")

    # Compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

    # Hide symbols by default for cleaner libraries
    set(CMAKE_C_VISIBILITY_PRESET hidden)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
endif()

# ------------------------------------------------------------------------------
# macOS Frameworks
# ------------------------------------------------------------------------------
set(SYNTHFLOW_MACOS_FRAMEWORKS
    CoreFoundation
    Security
    SystemConfiguration
    Foundation
)

# Find and store framework paths
foreach(fw ${SYNTHFLOW_MACOS_FRAMEWORKS})
    find_library(${fw}_FRAMEWORK ${fw})
    if(${fw}_FRAMEWORK)
        list(APPEND SYNTHFLOW_MACOS_FRAMEWORK_PATHS ${${fw}_FRAMEWORK})
    endif()
endforeach()

# ------------------------------------------------------------------------------
# Application Bundle Configuration
# ------------------------------------------------------------------------------
set(SYNTHFLOW_BUNDLE_IDENTIFIER "dev.synthflow.SynthFlow")
set(SYNTHFLOW_BUNDLE_VERSION ${PROJECT_VERSION})
set(SYNTHFLOW_BUNDLE_SHORT_VERSION ${PROJECT_VERSION})

# Bundle icon
set(SYNTHFLOW_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/resources/synthflow.icns")

# ------------------------------------------------------------------------------
# Function to create a macOS application bundle
# ------------------------------------------------------------------------------
function(synthflow_create_macos_bundle target)
    if(NOT APPLE)
        return()
    endif()

    # Set bundle properties
    set_target_properties(${target} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME "SynthFlow"
        MACOSX_BUNDLE_GUI_IDENTIFIER "${SYNTHFLOW_BUNDLE_IDENTIFIER}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${SYNTHFLOW_BUNDLE_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${SYNTHFLOW_BUNDLE_SHORT_VERSION}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${SYNTHFLOW_BUNDLE_VERSION}"
        MACOSX_BUNDLE_ICON_FILE "synthflow.icns"
        MACOSX_BUNDLE_COPYRIGHT "Copyright (c) $(date +%Y) SynthFlow. All rights reserved."
        MACOSX_BUNDLE_EXECUTABLE_NAME "${target}"
    )

    # Add frameworks
    foreach(fw ${SYNTHFLOW_MACOS_FRAMEWORKS})
        find_library(${fw}_FW ${fw})
        if(${fw}_FW)
            target_link_libraries(${target} PRIVATE ${${fw}_FW})
        endif()
    endforeach()

    # Copy icon to Resources
    if(EXISTS ${SYNTHFLOW_BUNDLE_ICON})
        set_source_files_properties(${SYNTHFLOW_BUNDLE_ICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
        target_sources(${target} PRIVATE ${SYNTHFLOW_BUNDLE_ICON})
    endif()

    # Set output directory
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Applications"
    )
endfunction()

# ------------------------------------------------------------------------------
# Function to create a command-line tool (not a bundle)
# ------------------------------------------------------------------------------
function(synthflow_create_macos_cli_tool target)
    if(NOT APPLE)
        return()
    endif()

    # Set output directory
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    # Link frameworks
    foreach(fw ${SYNTHFLOW_MACOS_FRAMEWORKS})
        find_library(${fw}_FW ${fw})
        if(${fw}_FW)
            target_link_libraries(${target} PRIVATE ${${fw}_FW})
        endif()
    endforeach()
endfunction()

# ------------------------------------------------------------------------------
# DMG Configuration (Drag and Drop installer)
# ------------------------------------------------------------------------------
set(CPACK_DMG_FORMAT "UDBZ")  # Compressed DMG
set(CPACK_DMG_VOLUME_NAME "SynthFlow ${PROJECT_VERSION}")
set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/resources/dmg-background.png")
set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${CMAKE_SOURCE_DIR}/cmake/dmg-setup.scpt")

# Applications to place in DMG
set(CPACK_DMG_SLA_USE_RESOURCE_FILE_LICENSE ON)

# ------------------------------------------------------------------------------
# ProductBuild Configuration (PKG installer)
# ------------------------------------------------------------------------------
set(CPACK_PRODUCTBUILD_IDENTITY_NAME "SynthFlow")
set(CPACK_PRODUCTBUILD_IDENTIFIER "${SYNTHFLOW_BUNDLE_IDENTIFIER}")

# ------------------------------------------------------------------------------
# Code Signing Configuration
# ------------------------------------------------------------------------------
# For development, you can skip code signing:
# cmake -DSYNTHFLOW_SKIP_CODESIGN=ON ..
option(SYNTHFLOW_SKIP_CODESIGN "Skip code signing during development" OFF)

if(NOT SYNTHFLOW_SKIP_CODESIGN AND APPLE)
    # Look for signing identity
    execute_process(
        COMMAND security find-identity -v -p codesigning
        OUTPUT_VARIABLE SYNTHFLOW_SIGNING_IDENTITIES
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(SYNTHFLOW_SIGNING_IDENTITIES)
        # Extract first identity
        string(REGEX MATCH "\"([^\"]+)\"" SYNTHFLOW_SIGNING_IDENTITY "${SYNTHFLOW_SIGNING_IDENTITIES}")
        if(CMAKE_MATCH_1)
            set(SYNTHFLOW_CODESIGN_IDENTITY "${CMAKE_MATCH_1}" CACHE STRING "Code signing identity")
            message(STATUS "Found signing identity: ${SYNTHFLOW_CODESIGN_IDENTITY}")
        endif()
    endif()
endif()

# ------------------------------------------------------------------------------
# Function to codesign a target
# ------------------------------------------------------------------------------
function(synthflow_codesign target)
    if(APPLE AND SYNTHFLOW_CODESIGN_IDENTITY AND NOT SYNTHFLOW_SKIP_CODESIGN)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND codesign --force --sign "${SYNTHFLOW_CODESIGN_IDENTITY}"
                    --options runtime
                    --timestamp
                    --entitlements "${CMAKE_SOURCE_DIR}/cmake/entitlements.plist"
                    "$<TARGET_FILE:${target}>"
            COMMENT "Signing ${target}..."
        )
    endif()
endfunction()

# ------------------------------------------------------------------------------
# Function to notarize a target (requires Apple Developer account)
# ------------------------------------------------------------------------------
function(synthflow_notarize target)
    if(APPLE AND SYNTHFLOW_NOTARIZE AND SYNTHFLOW_CODESIGN_IDENTITY)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND xcrun notarytool submit "$<TARGET_FILE:${target}>"
                    --apple-id "${SYNTHFLOW_APPLE_ID}"
                    --team-id "${SYNTHFLOW_TEAM_ID}"
                    --password "${SYNTHFLOW_APP_PASSWORD}"
                    --wait
            COMMENT "Notarizing ${target}..."
        )
    endif()
endfunction()

# ------------------------------------------------------------------------------
# Function to apply macOS-specific settings to a target
# ------------------------------------------------------------------------------
function(synthflow_apply_macos_settings target)
    if(NOT APPLE)
        return()
    endif()

    # Link frameworks
    foreach(fw ${SYNTHFLOW_MACOS_FRAMEWORKS})
        find_library(${fw}_FW ${fw})
        if(${fw}_FW)
            target_link_libraries(${target} PRIVATE ${${fw}_FW})
        endif()
    endforeach()

    # Set deployment target
    target_compile_definitions(${target} PRIVATE
        __MACOSX_DEPLOYMENT_TARGET__=${CMAKE_OSX_DEPLOYMENT_TARGET}
    )

    # Install destination
    install(TARGETS ${target}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endfunction()

# ------------------------------------------------------------------------------
# Create Info.plist from template
# ------------------------------------------------------------------------------
function(synthflow_generate_infoplist output_path)
    set(INFO_PLIST_CONTENT
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">
<plist version=\"1.0\">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>en</string>
    <key>CFBundleExecutable</key>
    <string>synthflow</string>
    <key>CFBundleIconFile</key>
    <string>synthflow</string>
    <key>CFBundleIdentifier</key>
    <string>${SYNTHFLOW_BUNDLE_IDENTIFIER}</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>SynthFlow</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>${SYNTHFLOW_BUNDLE_SHORT_VERSION}</string>
    <key>CFBundleVersion</key>
    <string>${SYNTHFLOW_BUNDLE_VERSION}</string>
    <key>LSMinimumSystemVersion</key>
    <string>${CMAKE_OSX_DEPLOYMENT_TARGET}</string>
    <key>NSHumanReadableCopyright</key>
    <string>Copyright (c) $(date +%Y) SynthFlow. All rights reserved.</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>LSApplicationCategoryType</key>
    <string>public.app-category.developer-tools</string>
</dict>
</plist>"
    )
    file(WRITE ${output_path} "${INFO_PLIST_CONTENT}")
endfunction()

# ------------------------------------------------------------------------------
# Print macOS configuration summary
# ------------------------------------------------------------------------------
function(synthflow_print_macos_config)
    if(APPLE)
        message(STATUS "macOS Configuration:")
        message(STATUS "  Deployment Target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
        message(STATUS "  Architectures: ${CMAKE_OSX_ARCHITECTURES}")
        message(STATUS "  Bundle Identifier: ${SYNTHFLOW_BUNDLE_IDENTIFIER}")
        message(STATUS "  Code Signing: ${SYNTHFLOW_CODESIGN_IDENTITY}")
    endif()
endfunction()