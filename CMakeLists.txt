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
  "Engine/Source"
)

set(SYSTEM_INCLUDE_DIRS
  "Engine/Include"
  "Engine/ThirdParty"
)

file(GLOB_RECURSE SOURCES
  "Engine/Source/*.cpp"
  "Engine/ThirdParty/vma/*.cpp"
  "Engine/ThirdParty/volk/*.cpp"
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(GLFW_LINK_DIRECTORY "Engine/ThirdParty/GLFW/lib-mingw-w64")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(GLFW_LINK_DIRECTORY "Engine/ThirdParty/GLFW/lib-vc2022")
endif()

set(LINK_DIRECTORIES
  ${GLFW_LINK_DIRECTORY}
  "Engine/ThirdParty/vulkan"
)

set(LINK_LIBRARIES
  "glfw3"
  "vulkan-1"
)

include_directories(${INCLUDE_DIRS})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRS})
link_directories(${LINK_DIRECTORIES})

add_executable(${PROGRAM_NAME} "${SOURCES}")

target_link_libraries(${PROGRAM_NAME}
  PRIVATE
  ${LINK_LIBRARIES}
)