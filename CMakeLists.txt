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

include_directories(${PROGRAM_NAME} SYSTEM thirdparty)
include_directories(${PROGRAM_NAME} SYSTEM thirdparty/vulkan/Include)
include_directories(${PROGRAM_NAME} SYSTEM thirdparty/glfw/include)
include_directories(${PROGRAM_NAME} SYSTEM thirdparty/imgui)
include_directories(${PROGRAM_NAME} SYSTEM include)
include_directories(${PROGRAM_NAME} /)

link_directories(
    thirdparty/vulkan
    thirdparty/glfw/lib-mingw-w64
)

add_executable(${PROGRAM_NAME}
    main.cpp
    drivers/render_device_context.cpp
    drivers/render_device.cpp
    platform/window.cpp
    platform/win32/render_device_context_win32.cpp
    render/camera/camera.cpp
    render/camera/controller.cpp
    render/camera/track_ball_camera_controller.cpp
    render/camera/projection_camera.cpp
    render/renderer_imgui.cpp
    render/renderer_canvas.cpp
    render/renderer_screen.cpp
    # ============================ #
    #        implementation        #
    # ============================ #
    thirdparty/vma/vk_mem_alloc.cpp
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/imgui_demo.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/imgui_widgets.cpp
    thirdparty/imgui/backends/imgui_impl_vulkan.cpp
    thirdparty/imgui/backends/imgui_impl_glfw.cpp
)

target_link_libraries(${PROGRAM_NAME}
    glfw3
    vulkan-1
)