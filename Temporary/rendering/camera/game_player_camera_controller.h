/* ======================================================================== */
/* game_player_camera_controller.h                                          */
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
#ifndef _GAME_PLAYER_CAMERA_CONTROLLER_H_
#define _GAME_PLAYER_CAMERA_CONTROLLER_H_

#include "controller.h"

class GamePlayerCameraController : public CameraController {
public:
    virtual void on_event_mouse_button(int button, int action, int mods) override final;
    virtual void on_event_cursor(float x, float y) override final;
    virtual void on_event_key(int key, int action) override final;
    virtual void on_event_scroll(float x, float y) override final;
    virtual void on_update_camera() override final;

private:
    bool press[1024] = { 0 };
};

#endif /* _GAME_PLAYER_CAMERA_CONTROLLER_H_ */
