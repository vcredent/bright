#! ************************************************************************ !#
#! PortableMain.cpp                                                         !#
#! ************************************************************************ !#
#!                        This file is part of:                             !#
#!                           PORTABLE ENGINE                                !#
#! ************************************************************************ !#
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
#! ************************************************************************ !#

CMAKE_MINIMUM_REQUIRED(VERSION 3.26)
PROJECT(PortableEngine)

SET(CMAKE_CXX_STANDARD 23)

INCLUDE_DIRECTORIES(${PROJECT_NAME} SYSTEM Source/ThirdParty/vulkan/Include)

INCLUDE_DIRECTORIES(${PROJECT_NAME} Source)
INCLUDE_DIRECTORIES(${PROJECT_NAME} Source/Engine)

LINK_DIRECTORIES(
    Source/ThirdParty/vulkan
)

ADD_EXECUTABLE(${PROJECT_NAME}
    Source/Engine/PortableMain.cpp
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    vulkan-1
)