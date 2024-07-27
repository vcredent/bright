/* ======================================================================== */
/* projection_camera.h                                                      */
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
#ifndef _PROJECTION_CAMERA_H_
#define _PROJECTION_CAMERA_H_

#include "camera.h"

class ProjectionCamera : public Camera {
public:
    ProjectionCamera(float v_fov = 45.0f, float v_near = 0.01f, float v_far = 65.0f, float v_aspect_ratio = 1.0f);
    ~ProjectionCamera();

    float get_fov() { return fov; }
    float get_near() { return near; }
    float get_far() { return far; }
    float get_aspect_ratio() { return aspect_ratio; }

    void set_fov(float v_fov) { fov = v_fov; }
    void set_near(float v_near) { near = v_near; }
    void set_far(float v_far) { far = v_far; }
    void set_aspect_ratio(float v_aspect_ratio) { aspect_ratio = v_aspect_ratio; }

    virtual Matrix4 look_view() override final;
    virtual Matrix4 perspective() override final;

private:
    float fov;
    float near;
    float far;
    float aspect_ratio;
};

#endif /* _PROJECTION_CAMERA_H_ */
