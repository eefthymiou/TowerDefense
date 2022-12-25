# Install script for directory: /Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library

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
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library/libSOIL_d.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSOIL_d.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSOIL_d.a")
    execute_process(COMMAND "/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSOIL_d.a")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SOIL" TYPE FILE FILES
    "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/include/SOIL.h"
    "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/include/image_DXT.h"
    "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/include/image_helper.h"
    "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/include/stb_image_aug.h"
    "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/include/stbi_DDS_aug.h"
    "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/include/stbi_DDS_aug_c.h"
    )
endif()

