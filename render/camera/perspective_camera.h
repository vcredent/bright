/* ======================================================================== */
/* perspective_camera.h                                                     */
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
#ifndef _GAME_PLAY_CAMERA_H_
#define _GAME_PLAY_CAMERA_H_

#include "camera.h"

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(float fov, float aspect, float near, float far);
   ~PerspectiveCamera();

    virtual glm::mat4 look_at() override final;
    virtual glm::mat4 perspective() override final;

    void get_perspective_far() { this->far; }
    void get_perspective_near() { this->near; }
    void get_perspective_fov() { this->fov; }

    void set_aspect_ratio(float aspect) { this->aspect_ratio = aspect; }
    void set_perspective_far(float far) { this->far = far; }
    void set_perspective_near(float near) { this->near = near; }
    void set_perspective_fov(float fov) { this->fov = fov; }

private:
    float aspect_ratio;
    float far;
    float near;
    float fov;
};

#endif /* _GAME_PLAY_CAMERA_H_ */