#! ======================================================================== !#
#! PortableMain.cpp                                                         !#
#! ======================================================================== !#
#!                        This file is part of:                             !#
#!                           COPILOT ENGINE                                 !#
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
project(copilot)

set(CMAKE_CXX_STANDARD 23)
set(PROGRAM_NAME copilot)

set(INCLUDE_DIRS
  "include"
  "thirdparty"
)

set(LINK_DIRS
  "thirdparty/GLFW/lib-mingw-w64"
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
target_include_directories(glfw_vulkan_links INTERFACE "thirdparty")

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