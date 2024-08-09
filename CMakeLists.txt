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
project(BrightEngine)

set(CMAKE_CXX_STANDARD 23)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options("/source-charset:utf-8")
  add_compile_options("/execution-charset:utf-8")
endif()

include_directories(
  "Engine/Include"
  "Engine/ThirdParty"
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(GLFW_LINK_DIRECTORY "Engine/ThirdParty/GLFW/lib-mingw-w64")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(GLFW_LINK_DIRECTORY "Engine/ThirdParty/GLFW/lib-vc2022")
endif()

link_directories(
  ${GLFW_LINK_DIRECTORY}
  "Engine/ThirdParty/vulkan"
)

add_subdirectory(Engine/Source/NavUI)
add_subdirectory(Engine/Source/Runtime)
add_subdirectory(Engine/Source/Sandbox)