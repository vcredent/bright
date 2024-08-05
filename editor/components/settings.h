/* ======================================================================== */
/* settings.h                                                               */
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
#ifndef _NAVEDITOR_COMPONENT_SETTINGS_H_
#define _NAVEDITOR_COMPONENT_SETTINGS_H_

#define _SELECTABLE(name, selected) ImGui::Selectable("        " name "    ", selected)
#define _CHECK_OPTION(current, option) if (current != option) return
#define _SETTINGS_INDENT() ImGui::Indent(32.0f)
#define _SETTINGS_UNINDENT() ImGui::Unindent(32.0f)

namespace _child {

    enum Options {
        None,
        SCENE_RENDER,
        ENGINE_SETTINGS
    };

    Options v_option = Options::None;

    static void _settings_scene_render(Naveditor::SettingValues *p_values)
    {
        _CHECK_OPTION(v_option, Options::SCENE_RENDER);
        static bool v;

        ImGui::SeparatorText("渲染");
        _SETTINGS_INDENT();
        ImGui::Checkbox("显示坐标线", &p_values->show_coordinate_render);
        _SETTINGS_UNINDENT();
    }

    // options
    static void options()
    {
        if (ImGui::CollapsingHeader("通用设置")) {
            if (_SELECTABLE("场景渲染", v_option == Options::SCENE_RENDER)) {
                v_option = SCENE_RENDER;
            }
        }

        ImGui::Separator();

        if (_SELECTABLE("引擎设置", v_option == Options::ENGINE_SETTINGS)) {
            v_option = ENGINE_SETTINGS;
        }
    }

    // value set
    static void values(Naveditor::SettingValues *p_values)
    {
        _settings_scene_render(p_values);
    }

}

static void _draw_engine_settings_editor_ui(bool *p_enable, Naveditor::SettingValues *p_values)
{
    if (*p_enable) {
        ImGui::Begin("引擎设置", p_enable);
        {
            ImGui::BeginChild("选项", ImVec2(150, 0), true);
            _child::options();
            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginChild("设置", ImVec2(0, 0), true);
            _child::values(p_values);
            ImGui::EndChild();
        }
        ImGui::End();
    }
}

#endif /* _NAVEDITOR_COMPONENT_SETTINGS_H_ */
