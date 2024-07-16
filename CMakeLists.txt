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
#! WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, e1ither express or implied !#
#! See the License for the specific language governing permissions and      !#
#! limitations under the License.                                           !#
#!                                                                          !#
#! ======================================================================== !#

cmake_minimum_required(VERSION 3.26)
project(copilot)

set(CMAKE_CXX_STANDARD 23)
set(PROGRAM_NAME engine)

include_directories(${PROJECT_NAME} SYSTEM thirdparty)
include_directories(${PROJECT_NAME} SYSTEM thirdparty/vulkan/Include)
include_directories(${PROJECT_NAME} SYSTEM thirdparty/glfw/include)
include_directories(${PROJECT_NAME} SYSTEM include)
include_directories(${PROJECT_NAME} /)

link_directories(
    thirdparty/vulkan
    thirdparty/glfw/lib-mingw-w64
)

add_executable(${PROGRAM_NAME}
    main.cpp
    drivers/rendering_context_driver_vulkan.cpp
    platform/win32/rendering_context_driver_vulkan_win32.cpp
    # ============================ #
    #        implementation        #
    # ============================ #
    thirdparty/vma/vk_mem_alloc.cpp
)

target_link_libraries(${PROGRAM_NAME}
    glfw3
    vulkan-1
)