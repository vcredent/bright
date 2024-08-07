#! ======================================================================== !#
#! PortableMain.cpp                                                         !#
#! ======================================================================== !#
#!                        This file is part of:                             !#
#!                            BRIGHT ENGINE                                 !#
#! ======================================================================== !#
#!                                                                          !#
#! Copyright (C) 2022 Vcredent All rights reserved.                         !#
#!                                                                          !#
#! Licensed under the Apache License, Version 2.0 (the "License");          !#
#! you may not use this file except in compliance with the License.         !#
#!                                                                          !#
#! You may obtain a copy of the License at                                  !#
#!     http://www.apache.org/licenses/LICENSE-2.0                           !#
#!                                                                          !#
#! Unless required by applicable law or agreed to in writing, software      !#
#! distributed under the License is distributed on an "AS IS" BASIS,        !#
#! WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied  !#
#! See the License for the specific language governing permissions and      !#
#! limitations under the License.                                           !#
#!                                                                          !#
#! ======================================================================== !#
cmake_minimum_required(VERSION 3.26)
project(bright)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options("/source-charset:utf-8")
  add_compile_options("/execution-charset:utf-8")
endif()

set(CMAKE_CXX_STANDARD 23)
set(PROGRAM_NAME bright)

set(INCLUDE_DIRS
  "include"
  "thirdparty"
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(glfw_link_dir "thirdparty/GLFW/lib-mingw-w64")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(glfw_link_dir "thirdparty/GLFW/lib-vc2022")
endif()

set(LINK_DIRS
  ${glfw_link_dir}
  "thirdparty/vulkan"
)

set(LINK_LIBS
  "navui"
  "glfw_vulkan_links"
)

file(GLOB_RECURSE SOURCES
  "main.cpp"
  "drivers/*.cpp"
  "editor/*.cpp"
  "misc/*.cpp"
  "platform/*.cpp"
  "modules/*.cpp"
  "rendering/*.cpp"
  "misc/*.cpp"
  "thirdparty/vma/*.cpp"
  "thirdparty/tinyobjloader/*.cpp"
  "thirdparty/stb/*.cpp"
)

link_directories(${LINK_DIRS})

# definition global link libraries and includes
add_library(glfw_vulkan_links INTERFACE)
target_link_libraries(glfw_vulkan_links INTERFACE
  "glfw3"
  "vulkan-1"
)
target_include_directories(glfw_vulkan_links INTERFACE "include" "thirdparty")

add_subdirectory(navui)

add_executable(${PROGRAM_NAME}
  "${SOURCES}"
)

target_include_directories(${PROGRAM_NAME}
  SYSTEM PRIVATE ${INCLUDE_DIRS}
)

target_include_directories(${PROGRAM_NAME}
  PRIVATE /
)

target_link_libraries(${PROGRAM_NAME}
  PRIVATE ${LINK_LIBS}
)