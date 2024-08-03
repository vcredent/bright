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
  "thirdparty"
  "thirdparty/vulkan/Include"
  "thirdparty/glfw/include"
  "thirdparty/imgui"
  "include"
)

set(LINK_LIBS
  "glfw3"
  "vulkan-1"
)

file(GLOB_RECURSE SOURCES
  "main.cpp"
  "drivers/*.cpp"
  "platform/*.cpp"
  "model/*.cpp"
  "rendering/*.cpp"
  "misc/*.cpp"
)

link_directories(${PROGRAM_NAME}
  "thirdparty/vulkan"
  "thirdparty/glfw/lib-mingw-w64"
)

add_executable(${PROGRAM_NAME}
  "${SOURCES}"
  # ============================ #
  #        implementation        #
  # ============================ #
  "thirdparty/imgui/imgui.cpp"
  "thirdparty/imgui/imgui_draw.cpp"
  "thirdparty/imgui/imgui_demo.cpp"
  "thirdparty/imgui/imgui_tables.cpp"
  "thirdparty/imgui/imgui_widgets.cpp"
  "thirdparty/imgui/backends/imgui_impl_vulkan.cpp"
  "thirdparty/imgui/backends/imgui_impl_glfw.cpp"
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

add_custom_command(
  TARGET ${PROGRAM_NAME}
  POST_BUILD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/shader
  COMMAND ${CMAKE_SOURCE_DIR}/shader/compile
)