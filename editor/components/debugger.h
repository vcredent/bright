/* ======================================================================== */
/* camera.h                                                                 */
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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, e1ither express or implied */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#ifndef _NAVEDITOR_COMPONENT_DEBUGGER_H_
#define _NAVEDITOR_COMPONENT_DEBUGGER_H_

static void _draw_debugger_editor_ui(DebuggerProperties *v_debugger)
{
    static std::vector<float> fps_list;

    fps_list.push_back(v_debugger->fps);
    if (std::size(fps_list) > 255)
        fps_list.erase(fps_list.begin());

    NavUI::Begin("调试");
    {
        ImGui::SeparatorText("高级信息");
        ImGui::Indent(32.0f);
        ImGui::Text("canvas render time: %.2fms", v_debugger->scene_render_time);
        ImGui::Text("screen render time: %.2fms", v_debugger->screen_render_time);
        ImGui::Text("total render time: %.2fms", v_debugger->scene_render_time + v_debugger->screen_render_time);
        ImGui::Unindent(32.0f);

        ImGui::SeparatorText("基础信息");
        ImGui::Indent(32.0f);
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "fps: %d", v_debugger->fps
        );
        ImGui::Indent(12.0f);
        ImGui::PlotLines("##", std::data(fps_list), std::size(fps_list), 0, NULL, 0.0f, 144.0f, ImVec2(0.0f, 32.0f));
        ImGui::Unindent(12.0f);
        ImGui::Unindent(32.0f);

        std::vector<Debugger::Temporary> temporary = Debugger::get_temporaries();
        if (!temporary.empty()) {
            ImGui::SeparatorText("临时数据调试");
            for (const auto& tmp : temporary) {
                switch (tmp.type) {
                    case Debugger::ValueType::FLOAT : NavUI::DragFloat(tmp.name, (float*) tmp.ptr);  break;
                    case Debugger::ValueType::FLOAT3: NavUI::DragFloat3(tmp.name, (float*) tmp.ptr); break;
                }
            }
        }

    }
    NavUI::End();
}

#endif /* _NAVEDITOR_COMPONENT_DEBUGGER_H_ */
