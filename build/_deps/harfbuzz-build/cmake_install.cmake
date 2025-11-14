# Install script for directory: /home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/harfbuzz" TYPE FILE FILES
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-aat-layout.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-aat.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-blob.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-buffer.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-common.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-cplusplus.hh"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-deprecated.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-draw.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-face.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-font.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-map.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-ot-color.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-ot-deprecated.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-ot-font.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-ot-layout.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-ot-math.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-ot-meta.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-ot-metrics.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-ot-name.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-ot-shape.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-ot-var.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-ot.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-paint.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-set.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-shape-plan.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-shape.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-style.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-unicode.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-version.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb.h"
    "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-src/src/hb-subset.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-build/libharfbuzz.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "pkgconfig" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-build/harfbuzz.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/harfbuzz/harfbuzzConfig.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/harfbuzz/harfbuzzConfig.cmake"
         "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-build/CMakeFiles/Export/6988f0906c47366608790bc51d4c19aa/harfbuzzConfig.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/harfbuzz/harfbuzzConfig-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/harfbuzz/harfbuzzConfig.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/harfbuzz" TYPE FILE FILES "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-build/CMakeFiles/Export/6988f0906c47366608790bc51d4c19aa/harfbuzzConfig.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/harfbuzz" TYPE FILE FILES "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-build/CMakeFiles/Export/6988f0906c47366608790bc51d4c19aa/harfbuzzConfig-noconfig.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-build/libharfbuzz-subset.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "pkgconfig" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-build/harfbuzz-subset.pc")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/runner/work/ImBored/ImBored/build/_deps/harfbuzz-build/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
