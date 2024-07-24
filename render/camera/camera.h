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
    virtual glm::mat4 look_at() = 0; /* calculation view matrix */
    virtual glm::mat4 perspective() = 0; /* calculation projection matrix */

    glm::vec3 get_position() { return position; }
    glm::vec3 get_right() { return right; }
    glm::vec3 get_up() { return up; }
    float get_yaw() { return yaw; }
    float get_pitch() { return pitch; }
    float get_roll() { return roll; }
    float get_aspect_ratio() { return aspect_ratio; }
    float get_far() { return far; }
    float get_near() { return near; }
    float get_fov() { return fov; }

    void set_position(glm::vec3 &position) { this->position = position; }
    void set_right(glm::vec3 &right) { this->right = right; }
    void set_up(glm::vec3 &up) { this->up = up; }
    void set_yaw(float yaw) { this->yaw = yaw; }
    void set_pitch(float pitch) { this->pitch = pitch; }
    void set_roll(float roll) { this->roll = roll; }
    void set_aspect_ratio(float aspect) { aspect_ratio = aspect; }
    void set_far(float far) { this->far = far; }
    void set_near(float near) { this->near = near; }
    void set_fov(float fov) { this->fov = fov; }

protected:
    /* the local_* variable is camera self local coordinate system */
    glm::vec3 position;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 direction;
    float yaw;
    float pitch;
    float roll;
    float far;
    float near;
    float fov;
    float aspect_ratio;
};

#endif /* _CAMERA_H_ */