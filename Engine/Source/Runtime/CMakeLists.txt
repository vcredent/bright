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
set(LIBRARY_NAME Runtime)
set(THIRD_PARTY_DIRECTORY "../../ThirdParty")

file(GLOB_RECURSE SOURCES
  "*.cpp"
  "${THIRD_PARTY_DIRECTORY}/vma/*.cpp"
  "${THIRD_PARTY_DIRECTORY}/volk/*.cpp"
)

set(LINK_LIBRARIES
  "glfw3"
  "vulkan-1"
)

include_directories(SYSTEM "Include")

link_directories(${LINK_DIRECTORIES})

add_library(${LIBRARY_NAME} STATIC "${SOURCES}")

target_include_directories(${LIBRARY_NAME} SYSTEM PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(${LIBRARY_NAME}
  PUBLIC
  ${LINK_LIBRARIES}
)