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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied  */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#include "projection_camera.h"

ProjectionCamera::ProjectionCamera(float v_fov, float v_near, float v_far)
{
   fov = v_fov;
   near = v_near;
   far = v_far;

   NodeGroup *group = get_node_group("属性");
   group->add_property("景深", NodePropertyType::FLOAT, &fov);
   group->add_property("近点", NodePropertyType::FLOAT, &near);
   group->add_property("远点", NodePropertyType::FLOAT, &far);
}

ProjectionCamera::~ProjectionCamera()
{
    /* do nothing... */
}

void ProjectionCamera::update()
{
    front.x = -glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    front.y = -glm::sin(glm::radians(pitch));
    front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    front = glm::normalize(front);

    view_matrix = glm::lookAt(position, position + front, world_up);

    projection_matrix = glm::perspective(glm::radians(fov), aspect_ratio, near, far);
    projection_matrix[1][1] *= -1;
}