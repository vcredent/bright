/* ======================================================================== */
/* camera_controller.cpp                                                    */
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
#include "controller.h"

void CameraController::on_update()
{
    /* do nothing... */
}

void CameraController::on_event_mouse(int button, int action, double x, double y)
{
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            mouse_left_key = action;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            mouse_left_key = action;
            break;
        default:
            return;
    }
}

void CameraController::on_event_cursor(float x, float y)
{
    this->mouse_position_x = x;
    this->mouse_position_y = y;
}

void CameraController::on_event_key(int key, bool action, int modes)
{
    if (key_events.count(key)) {
        key_events[key].action = action;
        key_events[key].modes = modes;
        return;
    }

    KeyEvent event = {
         /* key= */ key,
         /* action= */ action,
         /* modes= */ modes,
    };

    /* save key */
    key_events.insert(std::pair<int, KeyEvent>(key, event));
}

CameraController::KeyEvent *CameraController::getkey(int key)
{
    return &key_events[key];
}