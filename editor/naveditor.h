/* ======================================================================== */
/* naveditor.h                                                              */
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
#ifndef _NAVEDITOR_H_
#define _NAVEDITOR_H_

#include <navui/navui.h>
#include <copilot/debugger.h>
#include <vector>

// rendering
#include "rendering/rendering_screen.h"
#include "rendering/camera/camera.h"

class Naveditor {
public:
    U_MEMNEW_ONLY Naveditor(RenderDevice *v_rd, RenderingScreen *v_screen);
   ~Naveditor();

    // begin and end render
    void cmd_begin_naveditor_render(VkCommandBuffer cmd_buffer);
    void cmd_end_naveditor_render(VkCommandBuffer cmd_buffer);

    // api
    void cmd_draw_debugger_editor_ui();
    void cmd_draw_camera_editor_ui(Camera *v_camera);
    void cmd_draw_scene_viewport_ui(RenderDevice::Texture2D *v_texture, RenderDevice::Texture2D *v_depth, ImVec2 *p_region);

private:
    // manager window context and click state etc...
    struct Mger {
        bool enable_imgui_demo = false;
        bool enable_scene_settings = false;
    };

    void _draw_main_editor();

    Mger mger;
    DebuggerProperties *debugger;
    std::vector<float> fps_list;
};

#endif /* _NAVEDITOR_H_ */
