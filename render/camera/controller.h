/* ======================================================================== */
/* camera_controller.h                                                      */
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
#ifndef _CAMERA_CONTROLLER_H_
#define _CAMERA_CONTROLLER_H_

#include "platform/window.h"
#include <unordered_map>
#include "camera.h"

class CameraController {
public:
    struct KeyEvent {
        int key;
        bool action;
        int modes;
    };

    virtual void on_update();
    virtual void on_event_mouse(int button, int action, double x, double y);
    virtual void on_event_cursor(float x, float y);
    virtual void on_event_key(int key, bool action, int modes);

    void make_current_camera(Camera *camera) { this->camera = camera; }

protected:
    bool mouse_left_key;
    bool mouse_right_key;
    float mouse_position_x;
    float mouse_position_y;
    Camera *camera = nullptr;

    /* get key event */
    KeyEvent *getkey(int key);

private:
    std::unordered_map<int, KeyEvent> key_events;
};

#endif /* _CAMERA_CONTROLLER_H_ */
