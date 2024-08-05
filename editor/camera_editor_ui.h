/* ======================================================================== */
/* camera_editor_ui.h                                                       */
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
#ifndef _CAMERA_EDITOR_UI_H_
#define _CAMERA_EDITOR_UI_H_

#include "rendering/camera/camera.h"
#include <navui/navui.h>

namespace naveditor {
    static void draw_camera_editor_ui(Camera *v_camera)
    {
        NavUI::Begin("摄像机");
        {
            ImGui::SeparatorText("变换");
            Vec3 position = v_camera->get_position();
            NavUI::DragFloat3("位置: ", glm::value_ptr(position), 0.01f);
            v_camera->set_position(position);

            Vec3 target = v_camera->get_target();
            NavUI::DragFloat3("目标: ", glm::value_ptr(target), 0.01f);
            v_camera->set_target(target);

            float fov = v_camera->get_fov();
            NavUI::DragFloat("景深: ", &fov, 0.01f);
            v_camera->set_fov(fov);

            float near = v_camera->get_near();
            NavUI::DragFloat("近点: ", &near, 0.01f);
            v_camera->set_near(near);

            float far = v_camera->get_far();
            NavUI::DragFloat("远点: ", &far, 0.01f);
            v_camera->set_far(far);

            float speed = v_camera->get_speed();
            NavUI::DragFloat("速度: ", &speed, 0.01f);
            v_camera->set_speed(speed);
        }
        NavUI::End();
    }
}

#endif /* _CAMERA_EDITOR_UI_H_ */
