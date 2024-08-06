/* ======================================================================== */
/* scene_node_browser.h                                                     */
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
#ifndef _NAVEDITOR_COMPONENT_SCENE_BROWSER_H_
#define _NAVEDITOR_COMPONENT_SCENE_BROWSER_H_

struct _NodeSelected {
    const char* name = NULL;
    ClassProperties* node = NULL;
};

static void _cmd_draw_scene_node_browser(const std::vector<ClassProperties*>& v_properties, Naveditor* naveditor)
{
    static _NodeSelected current;

    if (current.name == NULL && !v_properties.empty())
        current = { v_properties[0]->get_node_name(), v_properties[0] };

    if (NavUI::Begin("节点浏览器")) {
        ImGui::Indent(32.0f);
        for (const auto& item : v_properties) {
            Naveditor::Navicon* icon = naveditor->geticon(item->get_icon());
            const char* name = item->get_node_name();
            NavUI::DrawTexture(icon->texture, ImVec2(18.0f, 18.0f));
            ImGui::SameLine();
            if (ImGui::Selectable(name, (current.name == name))) {
                current = { item->get_node_name(), item };
            }
        }
        ImGui::Unindent(32.0f);
        NavUI::End();
    }

    if (current.name != NULL) {
        NavUI::Begin("属性面板");
        ClassProperties* node = current.node;
        auto properties = node->get_properties();
        for (const auto& item : properties) {
            const char* name = item.first;
            ClassProperties::Property property = item.second;
            switch (property.type) {
            case FLOAT: NavUI::DragFloat(name, (float*) property.ptr, 0.01f); break;
            case FLOAT2: NavUI::DragFloat2(name, (float*) property.ptr, 0.01f); break;
            case FLOAT3: NavUI::DragFloat3(name, (float*) property.ptr, 0.01f); break;
            }
        }
        NavUI::End();
    }
}

#endif /* _NAVEDITOR_COMPONENT_SCENE_BROWSER_H_ */
