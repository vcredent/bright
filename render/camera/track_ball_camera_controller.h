/* ======================================================================== */
/* track_ball_camera_controller.h                                           */
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
#ifndef _TRACK_BALL_CAMERA_CONTROLLER_H_
#define _TRACK_BALL_CAMERA_CONTROLLER_H_

#include "controller.h"

class TrackBallCameraController : public CameraController {
public:
    TrackBallCameraController(Camera *v_camera = NULL);
    ~TrackBallCameraController();

    virtual void on_update_camera() override final;

private:
    bool _check_update();

    bool dragging = false;
    float last_cursor_xpos = 0.0f;
    float last_cursor_ypos = 0.0f;
};

#endif /* _TRACK_BALL_CAMERA_CONTROLLER_H_ */
