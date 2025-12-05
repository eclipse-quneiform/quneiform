#############################################################################
# Name:        CMakeLists.txt
# Purpose:     Build script for Quneiform (GUI version)
# Author:      Blake Madden
# Created:     2024-08-17
# Copyright:   (c) 2021-2025 Blake Madden
# License:     3-Clause BSD license
#############################################################################

# Declare the minimum required CMake version
cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_CONFIGURATION_TYPES)
    get_property(HAVE_MULTI_CONFIG_GENERATOR GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    # Set default configuration types for multi-config generators
    if(HAVE_MULTI_CONFIG_GENERATOR)
        set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
    endif()
endif()

# https://blog.kitware.com/cmake-and-the-default-build-type/
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(default_build_type "Debug")
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

message(STATUS "Build: ${CMAKE_BUILD_TYPE}")
message(STATUS "Generator: ${CMAKE_GENERATOR}")

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
elseif(UNIX)
    set(UNIX_ARCHITECTURE "x86_64" CACHE STRING "" FORCE)
endif()

if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    # If no deployment target has been set default to the minimum supported
    # OS version (this has to be set before the first project() call)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0 CACHE STRING "iOS Deployment Target")
    else()
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "macOS Deployment Target")
    endif()
    message(STATUS "macOS deployment target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
endif()

project(quneiform VERSION 2026.0.0.0 DESCRIPTION "Eclipse Quneiform")
set(PROGRAM_LONGNAME ${quneiform_DESCRIPTION})
set(PROGRAM_NUMBERED_VERSION 1.0)
set(PUBLISHER "Eclipse Foundation AISBL")
set(PROGRAM_AUTHOR "Blake Madden")
set(PROGRAM_COMMENTS "Internationalization & localization analysis system")
set(PROGRAM_RDNS_IDENTIFIER "org.eclipse.quneiform")
set(INNOSETUP_GUID "{D56F7D47-08F0-42D4-9A7B-A892D72CBE5A}")
set(PROGRAM_LICENSE "BSD-3-Clause")

if(CMAKE_COMPILER_IS_GNUCC)
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "12.2.1")
    message(FATAL_ERROR "Insufficient GCC version")
  endif()
endif()

# Build a displayable version string that only includes the necessary parts
set(FULL_DISPLAY_VERSION ${quneiform_VERSION_MAJOR})
if(NOT ${quneiform_VERSION_MINOR} EQUAL 0 OR NOT ${quneiform_VERSION_PATCH} EQUAL 0 OR NOT ${quneiform_VERSION_TWEAK} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${quneiform_VERSION_MINOR}")
endif()
if(NOT ${quneiform_VERSION_PATCH} EQUAL 0 OR NOT ${quneiform_VERSION_TWEAK} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${quneiform_VERSION_PATCH}")
endif()
if(NOT ${quneiform_VERSION_TWEAK} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${quneiform_VERSION_TWEAK}")
endif()
string(TIMESTAMP BUILD_YEAR "%Y")

message(STATUS "${quneiform_DESCRIPTION}")
message(STATUS "${PROGRAM_COMMENTS}")
message(STATUS "Full version: ${quneiform_VERSION_MAJOR}.${quneiform_VERSION_MINOR}.${quneiform_VERSION_PATCH}.${quneiform_VERSION_TWEAK}")
message(STATUS "Display version: ${FULL_DISPLAY_VERSION}")
message(STATUS "Numbered Version: ${PROGRAM_NUMBERED_VERSION}")
message(STATUS "Built in: ${BUILD_YEAR}")
message(STATUS "Authored by: ${PROGRAM_AUTHOR}")
message(STATUS "Published by: ${PUBLISHER}")
message(STATUS "Homepage: ${quneiform_HOMEPAGE_URL}")
message(STATUS "Output program file: ${CMAKE_PROJECT_NAME}")
message(STATUS "Writing program info to build peripheral files")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resource.h.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/resource.h")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../docs/manual/_quarto.yml.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/../docs/manual/_quarto.yml")
if(UNIX AND NOT APPLE)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../installers/unix/AppDir/quneiform.desktop.in"
                   "${CMAKE_CURRENT_SOURCE_DIR}/../installers/unix/AppDir/quneiform.desktop")
    # AppImage tools have issues with building correct appdata.xml files, so turn this off for now
    # configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../installers/unix/AppDir/usr/share/metainfo/quneiform.appdata.xml.in"
    #               "${CMAKE_CURRENT_SOURCE_DIR}/../installers/unix/AppDir/usr/share/metainfo/quneiform.appdata.xml")
elseif(WIN32)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../installers/windows/setup.isl.in"
                   "${CMAKE_CURRENT_SOURCE_DIR}/../installers/windows/setup.isl")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../installers/windows/icons.isl.in"
                   "${CMAKE_CURRENT_SOURCE_DIR}/../installers/windows/icons.isl")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../installers/windows/quneiform.iss.in"
                   "${CMAKE_CURRENT_SOURCE_DIR}/../installers/windows/quneiform.iss")
endif()

# Build the documentation
#########################
find_package(Doxygen)
if(TARGET doxygen-docs)
    if(NOT DOXYGEN_FOUND)
        message(STATUS "Doxygen not found. API documentation will not be generated.")
    endif()
endif()

add_custom_target(doxygen-docs
            COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_SOURCE_DIR}/../docs/doxygen/Doxyfile"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/../docs/doxygen"
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)

# Setup wxWidgets (can be 1-5 folders up from here)
########################
include(../cmake/folders.cmake)

set(PROJECT_WX_DEBUG_LEVEL 1)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(PROJECT_WX_DEBUG_LEVEL 0)
else()
    set(PROJECT_WX_DEBUG_LEVEL 1)
endif()

set(wxUSE_WEBVIEW_EDGE ON CACHE BOOL "Use Edge backend for WebView (MSW)" FORCE)
set(wxBUILD_SHARED OFF CACHE BOOL "Build wxWidgets statically" FORCE)
set(wxBUILD_DEBUG_LEVEL ${PROJECT_WX_DEBUG_LEVEL} CACHE STRING "wxWidgets runtime debug level" FORCE)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(wxBUILD_OPTIMISE ON CACHE BOOL "Optimize wxWidgets for Release" FORCE)
    set(wxBUILD_STRIPPED_RELEASE ON CACHE BOOL "Strip symbols from wxWidgets Release builds" FORCE)
endif()
find_directory("wxWidgets" wxWidgets_BASEDIR)
if(NOT EXISTS "${wxWidgets_BASEDIR}")
    message(FATAL_ERROR "wxWidgets base directory not found: ${wxWidgets_BASEDIR}")
endif()
add_subdirectory(${wxWidgets_BASEDIR} ${CMAKE_CURRENT_SOURCE_DIR}/wxWidgets_lib)
set(wxWidgets_LIBRARIES wx::core wx::base wx::xrc wx::propgrid wx::xml wx::aui wx::ribbon wx::qa wx::stc wx::net wx::richtext)
message(STATUS "Adding wxWidgets libraries: ${wxWidgets_LIBRARIES}")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Build the app
########################
set(FILES ../src/analyze.cpp ../src/gui/i18napp.cpp ../src/gui/projectdlg.cpp ../src/i18n_string_util.cpp
          ../src/i18n_review.cpp ../src/input.cpp ../src/gui/datamodel.cpp ../src/gui/app_options.cpp
          ../src/cpp_i18n_review.cpp ../src/csharp_i18n_review.cpp  ../src/po_file_review.cpp
          ../src/translation_catalog_review.cpp ../src/rc_file_review.cpp ../src/gui/insert_transmacro_dlg.cpp
          ../src/gui/insert_translator_comment_dlg.cpp ../src/gui/convert_string_dlg.cpp
          ../src/gui/insert_warning_suppression_dlg.cpp
          ../src/info_plist_review.cpp ../src/quarto_review.cpp
          ../src/pseudo_translate.cpp ../src/gui/i18nframe.cpp ../src/gui/string_info_dlg.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

if(WIN32)
    # Include an RC file and HiDPI manifest for Windows (provided by wxWidgets)
    set(wxUSE_DPI_AWARE_MANIFEST 1)
    list(APPEND FILES ${CMAKE_CURRENT_SOURCE_DIR}/quneiform.rc)
    list(APPEND FILES "${wxWidgets_BASEDIR}/include/wx/msw/wx_dpi_aware.manifest")
elseif(APPLE)
    # Add an icon for the Apple .app file
    list(APPEND FILES ${CMAKE_SOURCE_DIR}/app-logo.icns)
endif()

# Define the build target for the executable
message(STATUS "Configuring program...")
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${FILES})

# Define wxDEBUG_LEVEL consistently for all platforms
target_compile_definitions(${PROJECT_NAME} PUBLIC wxDEBUG_LEVEL=${PROJECT_WX_DEBUG_LEVEL})

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXMSW__ _UNICODE _CRT_SECURE_NO_WARNINGS
    $<$<CONFIG:Debug>:__WXDEBUG__> $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${PROJECT_NAME} PUBLIC /Zc:__cplusplus /MP /W3 /WX /wd6211 /permissive- /sdl
                            $<$<CONFIG:Debug>:/Od>
                            $<$<CONFIG:Release>:/O2>)
elseif(MINGW OR MSYS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXMSW__ _UNICODE _CRT_SECURE_NO_WARNINGS
            $<$<CONFIG:Debug>:__WXDEBUG__> $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow -Werror
                           $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fstack-protector-strong>
                           $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-D_FORTIFY_SOURCE=2>
                           $<$<CONFIG:Debug>:-Og>
                           $<$<CONFIG:Release>:-O2>)
elseif(APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXOSX__
        $<$<CONFIG:Debug>:__WXDEBUG__> $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow -Werror
                           $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fstack-protector-strong>
                           $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-D_FORTIFY_SOURCE=2>
                           $<$<CONFIG:Debug>:-Og>
                           $<$<CONFIG:Release>:-O2>)
elseif(UNIX)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXGTK__
        $<$<CONFIG:Debug>:__WXDEBUG__> $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow -Werror
                           $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fstack-protector-strong>
                           $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-D_FORTIFY_SOURCE=2>
                           $<$<CONFIG:Debug>:-Og -g3>
                           $<$<CONFIG:Release>:-O2>)
    target_link_options(${PROJECT_NAME} PUBLIC
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wl,-z,relro,-z,now>)
endif()

# Link required libraries to the executable
if(WIN32 OR APPLE)
    # AppleClang dropped support for OpenMP
    # Including TBB is not necessary for macOS and actually breaks hardened runtime
    # because that library is not included by default with the OS (XCode installs it)
    # and it is not signed.
    target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
elseif(UNIX)
    # TBB is required for GCC linkage sometimes, although this app is not explicitly using it,
    # so try to link it in.
    find_package(TBB)
    if(TBB_FOUND)
        message(STATUS "Intel Threading Building Blocks: ${TBB_VERSION}")
        target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} TBB::tbb)
    else()
        target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
    endif()
endif()

# On macOS, move the resource files up from the binary folder to the resources folder
# (note that iOS has a flat folder structure, so only do this for macOS).
# Some of these files are compiled post build, and others are dynamic folders,
# so that is why we copy them like this post build instead of including them
# via RESOURCE like we do the icon.
if(APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(RESOURCE_FOLDER "/../resources")
else()
    set(RESOURCE_FOLDER "")
endif()

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/../docs/manual/book/ $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER})

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/images $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/images)

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_CURRENT_SOURCE_DIR}/../app-logo.svg $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/images)

if(UNIX AND NOT APPLE)
    # Icon used for Linux
    add_custom_command(TARGET ${PROJECT_NAME}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_CURRENT_SOURCE_DIR}/../app-logo.svg ${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir)
    # Application screenshot
    add_custom_command(TARGET ${PROJECT_NAME}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_CURRENT_SOURCE_DIR}/../docs/manual/images/main-window.png ${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir)
endif()
# License file
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER})

if(UNIX)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        C_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN TRUE)
endif()

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RESOURCE app-logo.icns
        # Name the app bundle "Quneiform"" (uppercase)
        OUTPUT_NAME "Quneiform"
        XCODE_ATTRIBUTE_EXECUTABLE_NAME ${PROJECT_NAME}
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)
endif()

# Copy application (and resources) to installer folders (if a production build)
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    if(WIN32)
        add_custom_command(TARGET ${PROJECT_NAME}
                           POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/release/"
                           COMMENT "Copying binary to installer folder.")
    elseif(UNIX AND NOT APPLE)
        add_custom_command(TARGET ${PROJECT_NAME}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/usr/bin)
        add_custom_command(TARGET ${PROJECT_NAME}
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/usr/bin"
                        COMMENT "Copying binary to installer folder.")
    elseif(APPLE)
        add_custom_command(TARGET ${PROJECT_NAME}
                           POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}/installers/macos/release/"
                           COMMENT "Copying binary to installer folder.")
    endif()
endif()
