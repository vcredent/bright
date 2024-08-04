/* ======================================================================== */
/* debugger_editor_ui.h                                                     */
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
#ifndef _DEBUGGER_EDITOR_UI_H_
#define _DEBUGGER_EDITOR_UI_H_

#include <navui.h>
#include <vector>
#include <copilot/memalloc.h>
#include <copilot/typedefs.h>

namespace naveditor {

    struct DebuggerStructure {
        int fps;
        std::vector<float> fps_list;
        float scene_render_time = 0.0f;
        float screen_render_time = 0.0f;
    };

    extern std::shared_ptr<DebuggerStructure> debugger;

    // add fps value
    static void add_debugger_fps_value(int fps)
    {
        debugger->fps = fps;
        debugger->fps_list.push_back(fps);
        if (std::size(debugger->fps_list) > 255) {
            debugger->fps_list.erase(debugger->fps_list.begin());
        }
    }

    static void draw_debugger_editor_ui()
    {
        NavUI::Begin("调试");
        {
            ImGui::SeparatorText("高级信息");
            ImGui::Indent(32.0f);
            ImGui::Text("canvas render time: %.2fms", debugger->scene_render_time);
            ImGui::Text("screen render time: %.2fms", debugger->screen_render_time);
            ImGui::Text("total render time: %.2fms", debugger->scene_render_time + debugger->screen_render_time);
            ImGui::Unindent(32.0f);

            ImGui::SeparatorText("基础信息");
            ImGui::Indent(32.0f);
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "fps: %d", debugger->fps
            );
            ImGui::Indent(12.0f);
            ImGui::PlotLines("##", std::data(debugger->fps_list), std::size(debugger->fps_list), 0, NULL, 0.0f, 144.0f, ImVec2(0.0f, 32.0f));
            ImGui::Unindent(12.0f);
            ImGui::Unindent(32.0f);
        }
        NavUI::End();
    }
}

#endif /* _DEBUGGER_EDITOR_UI_H_ */
