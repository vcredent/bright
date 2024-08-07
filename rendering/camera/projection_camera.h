/* ======================================================================== */
/* projection_camera.h                                                      */
/* ======================================================================== */
/*                        This file is part of:                             */
/*                            BRIGHT ENGINE                                 */
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
    ProjectionCamera(float v_fov = 45.0f, float v_near = 0.5f, float v_far = 6000.0f);
    ~ProjectionCamera();

    virtual void update() override final;
};

#endif /* _PROJECTION_CAMERA_H_ */
