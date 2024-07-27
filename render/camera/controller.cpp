/* ======================================================================== */
/* controller.cpp                                                           */
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
#include <copilot/memalloc.h>

CameraController::CameraController(Camera *v_camera)
    : camera(v_camera)
{
    mouse = (CameraController::MouseEvent *) imalloc(sizeof(CameraController::MouseEvent));
    cursor = (CameraController::CursorEvent *) imalloc(sizeof(CameraController::CursorEvent));
}


CameraController::~CameraController()
{
    free(mouse);
    free(cursor);
}

void CameraController::on_event_mouse_button(int button, int action, int mods)
{
    mouse->button = button;
    mouse->action = action;
    mouse->mods = mods;
}

void CameraController::on_event_cursor(float x, float y)
{
    cursor->x = x;
    cursor->y = y;
}

void CameraController::on_update_camera()
{
    /* do nothing... */
}