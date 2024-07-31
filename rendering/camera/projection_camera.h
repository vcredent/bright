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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied  */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#ifndef _PROJECTION_CAMERA_H_
#define _PROJECTION_CAMERA_H_

#include "camera.h"

class ProjectionCamera : public Camera {
public:
    ProjectionCamera(float v_fov = 45.0f, float v_near = 1.0f, float v_far = 65.0f);
    ~ProjectionCamera();

    V_FORCEINLINE float get_fov() { return fov; }
    V_FORCEINLINE float get_near() { return near; }
    V_FORCEINLINE float get_far() { return far; }

    V_FORCEINLINE void set_fov(float v_fov) { fov = v_fov; }
    V_FORCEINLINE void set_near(float v_near) { near = v_near; }
    V_FORCEINLINE void set_far(float v_far) { far = v_far; }

    virtual void update() override final;

private:
    float fov;
    float near;
    float far;
};

#endif /* _PROJECTION_CAMERA_H_ */
