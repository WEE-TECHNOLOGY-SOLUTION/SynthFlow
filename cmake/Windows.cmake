# ------------------------------------------------------------------------------
# Windows-specific CMake configuration for SynthFlow
# ------------------------------------------------------------------------------
# This module handles Windows-specific build settings including:
# - Static runtime linking for portable binaries
# - WiX Toolset configuration for MSI installers
# - NSIS configuration for executable installers
# - Windows-specific compiler flags
# ------------------------------------------------------------------------------

# Prevent multiple inclusion
if(SYNTHFLOW_WINDOWS_CMAKE_INCLUDED)
    return()
endif()
set(SYNTHFLOW_WINDOWS_CMAKE_INCLUDED TRUE)

# ------------------------------------------------------------------------------
# Windows-specific compiler flags
# ------------------------------------------------------------------------------
if(MSVC)
    # Enable multi-processor compilation for faster builds
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

    # Static runtime linking for portable binaries (no MSVC runtime dependency)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    # Additional useful MSVC flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")           # Exception handling
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")          # UTF-8 source encoding
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")  # Whole program optimization
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")  # Link-time code generation

    # Define Windows version for API compatibility
    add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX _WIN32_WINNT=0x0601)

    # Debug-specific definitions
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")

elseif(MINGW)
    # MinGW/GCC on Windows
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

    # Windows version definitions
    add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX _WIN32_WINNT=0x0601)
endif()

# ------------------------------------------------------------------------------
# Windows-specific libraries
# ------------------------------------------------------------------------------
set(SYNTHFLOW_WINDOWS_LIBS
    ws2_32      # Winsock2
    winhttp     # Windows HTTP services
    shell32     # Shell operations
    ole32       # COM library
    advapi32    # Advanced Windows API
)

# ------------------------------------------------------------------------------
# WiX Toolset Configuration (MSI Installer)
# ------------------------------------------------------------------------------
# WiX is used to create Windows Installer packages (.msi files)
# Requires WiX Toolset to be installed: https://wixtoolset.org/
# ------------------------------------------------------------------------------
set(CPACK_WIX_UPGRADE_GUID "E5B8A7D0-1C2F-4A3B-8D9E-0F1A2B3C4D5E")
set(CPACK_WIX_PRODUCT_GUID "A1B2C3D4-E5F6-7890-ABCD-EF1234567890")

# Product information
set(CPACK_WIX_PRODUCT_NAME "SynthFlow")
set(CPACK_WIX_PRODUCT_VERSION "${PROJECT_VERSION}")
set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/resources/synthflow.ico")

# Manufacturer/Vendor
set(CPACK_WIX_MANUFACTURER "SynthFlow")

# Installation directory
set(CPACK_WIX_INSTALL_DIR "ProgramFiles64Folder")

# UI settings
set(CPACK_WIX_UI_REF "WixUI_InstallDir")
set(CPACK_WIX_UI_BANNER "${CMAKE_SOURCE_DIR}/resources/banner.bmp")
set(CPACK_WIX_UI_DIALOG "${CMAKE_SOURCE_DIR}/resources/dialog.bmp")

# License file (optional)
if(EXISTS "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_WIX_LICENSE_RTF "${CMAKE_SOURCE_DIR}/LICENSE")
endif()

# Skip WIX UI features that require custom actions
set(CPACK_WIX_SKIP_WIX_UI_FEATURES OFF)

# Properties for Add/Remove Programs
set(CPACK_WIX_PROPERTY_ARPHELPLINK "https://synthflow.dev/docs")
set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://synthflow.dev")
set(CPACK_WIX_PROPERTY_ARPCONTACT "support@synthflow.dev")

# Patch files for custom WIX configuration
# set(CPACK_WIX_PATCH_FILE "${CMAKE_SOURCE_DIR}/cmake/wix-patch.xml")

# ------------------------------------------------------------------------------
# NSIS Configuration (Executable Installer)
# ------------------------------------------------------------------------------
# NSIS creates traditional Windows executable installers (.exe files)
# Requires NSIS to be installed: https://nsis.sourceforge.io/
# ------------------------------------------------------------------------------
set(CPACK_NSIS_PACKAGE_NAME "SynthFlow ${PROJECT_VERSION}")
set(CPACK_NSIS_DISPLAY_NAME "SynthFlow Programming Language")
set(CPACK_NSIS_HELP_LINK "https://synthflow.dev/docs")
set(CPACK_NSIS_URL_INFO_ABOUT "https://synthflow.dev")
set(CPACK_NSIS_CONTACT "support@synthflow.dev")

# Installation directory
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SynthFlow")

# Start menu integration
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

# Icons and graphics
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/resources/synthflow.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/resources/synthflow.ico")
set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP "${CMAKE_SOURCE_DIR}/resources/welcome.bmp")
set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP "${CMAKE_SOURCE_DIR}/resources/welcome.bmp")

# License file
if(EXISTS "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_NSIS_MUI_LICENSEFILE "${CMAKE_SOURCE_DIR}/LICENSE")
endif()

# Create desktop shortcut option
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\SynthFlow.lnk' '$INSTDIR\\\\bin\\\\synthflow.exe'"
)

# Extra uninstall commands
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$SMPROGRAMS\\\\$START_MENU\\\\SynthFlow.lnk'"
)

# ------------------------------------------------------------------------------
# Function to apply Windows-specific settings to targets
# ------------------------------------------------------------------------------
function(synthflow_apply_windows_settings target)
    if(WIN32)
        # Link Windows libraries (using plain signature for consistency)
        target_link_libraries(${target} ${SYNTHFLOW_WINDOWS_LIBS})

        # Set output directory
        set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )

        # For MSVC, set the runtime library
        if(MSVC)
            set_property(TARGET ${target} PROPERTY
                MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        endif()

        # Add Windows definitions
        target_compile_definitions(${target} PRIVATE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
            _WIN32_WINNT=0x0601
        )

        # Install destination
        install(TARGETS ${target}
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION bin
            ARCHIVE DESTINATION lib
        )
    endif()
endfunction()

# ------------------------------------------------------------------------------
# Function to create a Windows service executable
# ------------------------------------------------------------------------------
function(synthflow_create_windows_service target)
    if(WIN32)
        set_target_properties(${target} PROPERTIES
            WIN32_EXECUTABLE TRUE
        )
    endif()
endfunction()

# ------------------------------------------------------------------------------
# Print Windows configuration summary
# ------------------------------------------------------------------------------
function(synthflow_print_windows_config)
    message(STATUS "Windows Configuration:")
    message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
    message(STATUS "  Static Runtime: Yes")
    message(STATUS "  WiX Upgrade GUID: ${CPACK_WIX_UPGRADE_GUID}")
    message(STATUS "  NSIS Installer: Enabled")
endfunction()