/* ======================================================================== */
/* camera.h                                                                 */
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
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    virtual glm::mat4 look_at() = 0;
    virtual glm::mat4 perspective() = 0;

    void set_camera_position(float x, float y, float z) { position = glm::vec3(x, y, z); }
    void set_camera_right(float x, float y, float z) { right = glm::vec3(x, y, z); }
    void set_camera_up(float x, float y, float z) { up = glm::vec3(x, y, z); }

protected:
    /* the local_* variable is camera self local coordinate system */
    glm::vec3 position;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 front; /* front = up x right */
};

#endif /* _CAMERA_H_ */