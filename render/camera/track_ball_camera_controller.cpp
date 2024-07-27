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

    Matrix4 rotation = Matrix4(1.0f);

    float sensitivity = camera->get_sensitivity();

    float xoffset = (cursor->x - last_cursor_xpos) * sensitivity;
    float yoffset = (cursor->y - last_cursor_ypos) * sensitivity;

    last_cursor_xpos = cursor->x;
    last_cursor_ypos = cursor->y;

    camera->set_pitch(camera->get_pitch() - yoffset);
    camera->set_yaw(camera->get_yaw() + xoffset);

    Quat pitch = glm::angleAxis(glm::radians(camera->get_pitch()), camera->get_right());
    Quat yaw = glm::angleAxis(glm::radians(camera->get_yaw()), camera->get_up());

    rotation = glm::mat4_cast(yaw * pitch);
    camera->set_action_on_view_matrix(rotation);
}

bool TrackBallCameraController::_check_update()
{
    return camera != NULL && (mouse->button == EVENT_INPUT_MOUSE_BUTTON_LEFT && mouse->action);
}