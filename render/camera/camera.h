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

#include <copilot/math.h>

class Camera {
public:
    virtual Mat4 look_view() = 0;
    virtual Mat4 perspective() = 0;

    Vec3 get_position() { return position; }
    Vec3 get_right() { return right; }
    Vec3 get_up() { return up; }
    float get_speed() { return speed; }

    void set_position(Vec3 v_position) { position = v_position; }
    void set_right(Vec3 v_right) { right = v_right; }
    void set_up(Vec3 v_up) { up = v_up; }
    void set_speed(float v_speed) { speed = v_speed; }

protected:
    Vec3 position = Vec3(0.0f, 0.0f, 3.0f);
    Vec3 right = Vec3(1.0f, 0.0f, 0.0f);
    Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
    float speed = 0.5f;
};

#endif /* _CAMERA_H_ */