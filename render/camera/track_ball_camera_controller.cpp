/* ======================================================================== */
/* track_ball_camera_controller.cpp                                         */
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
#include "track_ball_camera_controller.h"
#include <iostream>

TrackBallCameraController::TrackBallCameraController(Camera *v_camera)
    : CameraController(v_camera)
{
    /* do nothing... */
}

TrackBallCameraController::~TrackBallCameraController()
{
    /* do nothing... */
}

void TrackBallCameraController::on_update_camera()
{
    if (!_check_update())
        return;

    if (!continued) {
        last_xpos  = cursor->x;
        last_ypos  = cursor->y;
        continued = true;
    }

    float xpos = cursor->x;
    float ypos = cursor->y;

    float xoffset = xpos - last_xpos;
    float yoffset = last_ypos - ypos;

    float sensitivity = camera->get_sensitivity();
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    if (abs(ypos - last_ypos) > 1.0f)
        camera->set_pitch(camera->get_pitch() + yoffset);
    if (abs(xpos - last_xpos) > 1.0f)
        camera->set_yaw(camera->get_yaw() + xoffset);

    last_xpos  = xpos;
    last_ypos  = ypos;
}

bool TrackBallCameraController::_check_update()
{
    return camera != NULL;
}