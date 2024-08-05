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

static void _draw_engine_settings_editor_ui(bool *p_enable)
{
    if (*p_enable) {
        ImGui::Begin("选项");
        {
            ImGui::Text("这是选项列表");
        }
        ImGui::End();

        ImGui::Begin("设置", p_enable);
        {
            ImGui::Text("这是设置列表");
        }
        ImGui::End();
    }
}

#endif /* _NAVEDITOR_COMPONENT_SETTINGS_H_ */
