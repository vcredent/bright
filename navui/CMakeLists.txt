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
set(LIBRARY_NAME navui)

include_directories(SYSTEM
  "imgui"
)

add_library(${LIBRARY_NAME} STATIC
  "navui.cpp"
  "imgui/imgui.cpp"
  "imgui/imgui_draw.cpp"
  "imgui/imgui_demo.cpp"
  "imgui/imgui_tables.cpp"
  "imgui/imgui_widgets.cpp"
  "imgui/backends/imgui_impl_vulkan.cpp"
  "imgui/backends/imgui_impl_glfw.cpp"
)

target_link_libraries(${LIBRARY_NAME} PRIVATE graph_lib)

target_include_directories(${LIBRARY_NAME} SYSTEM PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui
)

target_compile_options(${LIBRARY_NAME} PRIVATE -Wall)