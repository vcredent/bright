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
#include <copilot/typedefs.h>
#include <algorithm>

class Camera {
public:
    virtual Matrix4 look_view() = 0;
    virtual Matrix4 perspective() = 0;

    V_FORCEINLINE inline Vector3 get_position() { return position; }
    V_FORCEINLINE inline Vector3 get_right() { return right; }
    V_FORCEINLINE inline Vector3 get_up() { return up; }
    V_FORCEINLINE inline float get_pitch() { return pitch; }
    V_FORCEINLINE inline float get_yaw() { return yaw; }
    V_FORCEINLINE inline float get_roll() { return roll; }
    V_FORCEINLINE inline float get_speed() { return speed; }
    V_FORCEINLINE inline float get_sensitivity() { return sensitivity; }

    V_FORCEINLINE inline void set_position(Vector3 v_position) { position = v_position; }
    V_FORCEINLINE inline void set_right(Vector3 v_right) { right = v_right; }
    V_FORCEINLINE inline void set_up(Vector3 v_up) { up = v_up; }
    V_FORCEINLINE inline void set_pitch(float v_pitch) { pitch = glm::clamp(v_pitch, -89.0f, 89.0f); }
    V_FORCEINLINE inline void set_yaw(float v_yaw) { yaw = v_yaw; }
    V_FORCEINLINE inline void set_roll(float v_roll) { roll = v_roll; }
    V_FORCEINLINE inline void set_speed(float v_speed) { speed = v_speed; }
    V_FORCEINLINE inline void set_action_on_view_matrix(Matrix4 action_on) { action_on_view_matrix = action_on; }
    V_FORCEINLINE inline void set_sensitivity(float v_sensitivity) { sensitivity = v_sensitivity; }

protected:
    Vector3 position = Vector3(0.0f, 0.0f, 3.0f);
    Vector3 right = Vector3(1.0f, 0.0f, 0.0f);
    Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
    float pitch = 0.0f;
    float yaw = -90.0f;
    float roll = 0.0f;

    float speed = 0.5f;
    float sensitivity = 0.05f;

    Matrix4 action_on_view_matrix = Matrix4(1.0f);
};

#endif /* _CAMERA_H_ */