/* ======================================================================== */
/* projection_camera.cpp                                                    */
/* ======================================================================== */
/*                        This file is part of:                             */
/*                           COPILOT ENGINE                                 */
/* ======================================================================== */
/*                                                                          */
/* Copyright (C) 2022 Vcredent All rights reserved.                         */
/*                                                                          */
/* Licensed under the Apache License, Version 2.0 (the "License");          */
/* you may not use this file except in compliance with the License.         */
/*                                                                          */
/* You may obtain a copy of the License at                                  */
/*     http://www.apache.org/licenses/LICENSE-2.0                           */
/*                                                                          */
/* Unless required by applicable law or agreed to in writing, software      */
/* distributed under the License is distributed on an "AS IS" BASIS,        */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, e1ither express or implied */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#include "projection_camera.h"

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far)
{
    position = glm::vec3(0.0f, 0.0f, 4.0f);
    right = glm::vec3(1.0f, 0.0f, 0.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->fov = fov;
    this->aspect_ratio = aspect;
    this->near = near;
    this->far = far;
}

PerspectiveCamera::~PerspectiveCamera()
{
    /* do nothing... */
}

glm::mat4 PerspectiveCamera::look_view()
{
    direction = glm::cross(up, right);
    return glm::lookAt(position, position + direction, up);
}

glm::mat4 PerspectiveCamera::perspective()
{
    return glm::perspective(glm::radians(fov), aspect_ratio, near, far);
}