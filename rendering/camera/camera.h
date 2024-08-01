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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied  */
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
    U_MEMNEW_ONLY Camera();
    virtual ~Camera() = default;

    V_FORCEINLINE inline Vec3& get_position() { return position; }
    V_FORCEINLINE inline Vec3& get_world_right() { return world_right; }
    V_FORCEINLINE inline Vec3& get_world_up() { return world_up; }
    V_FORCEINLINE inline Vec3& get_camera_up() { return up; }
    V_FORCEINLINE inline Vec3& get_target() { return target; }
    V_FORCEINLINE inline Vec3& get_front() { return front; }
    V_FORCEINLINE inline float get_fov() { return fov; }
    V_FORCEINLINE inline float get_near() { return near; }
    V_FORCEINLINE inline float get_far() { return far; }
    V_FORCEINLINE inline float get_pitch() { return pitch; }
    V_FORCEINLINE inline float get_yaw() { return yaw; }
    V_FORCEINLINE inline float get_roll() { return roll; }
    V_FORCEINLINE inline float get_aspect_ratio() { return aspect_ratio; }
    V_FORCEINLINE inline float get_speed() { return speed; }
    V_FORCEINLINE inline float get_sensitivity() { return sensitivity; }
    V_FORCEINLINE inline Mat4& get_view_matrix() { return view_matrix; }
    V_FORCEINLINE inline Mat4& get_projection_matrix() { return projection_matrix; }

    V_FORCEINLINE inline void set_position(Vec3 v_position) { position = v_position; }
    V_FORCEINLINE inline void set_world_right(Vec3 v_world_right) { world_right = v_world_right; }
    V_FORCEINLINE inline void set_world_up(Vec3 v_world_up) { world_up = v_world_up; }
    V_FORCEINLINE inline void set_target(Vec3 v_target) { target = v_target; }
    V_FORCEINLINE inline void set_front(Vec3 v_front) { front = v_front; }
    V_FORCEINLINE inline void set_fov(float v_fov) { fov = v_fov; }
    V_FORCEINLINE inline void set_near(float v_near) { near = v_near; }
    V_FORCEINLINE inline void set_far(float v_far) { far = v_far; }
    V_FORCEINLINE inline void set_pitch(float v_pitch) { pitch = v_pitch; }
    V_FORCEINLINE inline void set_yaw(float v_yaw) { yaw = v_yaw; }
    V_FORCEINLINE inline void set_roll(float v_roll) { roll = v_roll; }
    V_FORCEINLINE inline void set_aspect_ratio(float v_aspect_ratio) { aspect_ratio = v_aspect_ratio; }
    V_FORCEINLINE inline void set_speed(float v_speed) { speed = v_speed; }
    V_FORCEINLINE inline void set_sensitivity(float v_sensitivity) { sensitivity = v_sensitivity; }
    V_FORCEINLINE inline void set_view_matrix(Mat4 &v_view_matrix) { view_matrix = v_view_matrix; }
    V_FORCEINLINE inline void set_projection_matrix(Mat4 &v_projection_matrix) { projection_matrix = v_projection_matrix; }

    virtual void update() = 0;

protected:
    Vec3 position = Vec3(0.0f, 0.0f, 3.0f);
    Vec3 world_right = Vec3(1.0f, 0.0f, 0.0f);
    Vec3 world_up = Vec3(0.0f, -1.0f, 0.0f);
    Vec3 up = Vec3(0.0f, -1.0f, 0.0f);
    Vec3 target = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 front = Vec3(0.0f, 0.0f, -1.0f);

    float fov = 1.0f;
    float near = 1.0f;
    float far = 90.0f;
    float pitch = 0.0f;
    float yaw = -90.0f;
    float roll = 0.0f;
    float aspect_ratio = 1.0f;

    float speed = 0.005f;
    float sensitivity = 0.05f;

    Mat4 view_matrix = Mat4(1.0f);
    Mat4 projection_matrix = Mat4(1.0f);
};

#endif /* _CAMERA_H_ */