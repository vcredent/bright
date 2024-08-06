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

struct _ObjectSelected {
    const char *name = NULL;
    RenderObject *object = NULL;
};

static void _cmd_draw_scene_node_browser(std::vector<RenderObject *> *objects, ImTextureID icon_cube)
{
    static _ObjectSelected current;

    if (current.name == NULL && !objects->empty())
        current = { (*objects)[0]->get_name(), (*objects)[0] };

    if (NavUI::Begin("场景节点浏览器")) {
        ImGui::Indent(32.0f);
        for (const auto &item: *objects) {
            const char *name = item->get_name();
            NavUI::DrawTexture(icon_cube, ImVec2(18.0f, 18.0f));
            ImGui::SameLine();
            if (ImGui::Selectable(name, (current.name == name))) {
                current = { item->get_name(), item };
            }
        }
        ImGui::Unindent(32.0f);
        NavUI::End();
    }
}

#endif /* _NAVEDITOR_COMPONENT_SCENE_BROWSER_H_ */
