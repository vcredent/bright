/* ======================================================================== */
/* scene.h                                                              */
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
#ifndef _NAVEDITOR_COMPONENT_SCENE_H_
#define _NAVEDITOR_COMPONENT_SCENE_H_

static void _draw_scene_editor_ui(RenderDevice::Texture2D *v_texture, RenderDevice::Texture2D *v_depth, ImVec2 *p_region)
{
    NavUI::BeginViewport("场景");
    {
        static ImTextureID preview = NULL;
        if (preview != NULL)
            NavUI::RemoveTexture(preview);

        static ImTextureID depth = NULL;
        if (depth != NULL)
            NavUI::RemoveTexture(depth);

        preview = NavUI::AddTexture(v_texture->sampler, v_texture->image_view, v_texture->image_layout);
        depth = NavUI::AddTexture(v_depth->sampler, v_depth->image_view, v_depth->image_layout);

        // Main image
        {
            *p_region = ImGui::GetContentRegionAvail();
            ImGui::Image(preview, ImVec2(p_region->x, p_region->y));
        }

        // Depth image
        {
            ImVec2 position = ImGui::GetWindowPos();
            ImVec2 size = ImGui::GetWindowSize();

            ImVec2 offset = ImVec2(30.0f, 70.0f);
            ImVec2 tex_size = ImVec2((size.x * 0.1f) * 1.3f, (size.y * 0.1f) * 1.3f);

            ImVec2 depth_tex_pos = ImVec2(position.x + offset.x, position.y + size.y - tex_size.y - offset.y);

            ImDrawList *draw = ImGui::GetWindowDrawList();
            draw->AddImage(depth, depth_tex_pos, ImVec2(depth_tex_pos.x + tex_size.x, depth_tex_pos.y + tex_size.y));
        }
    }
    NavUI::EndViewport();
}

#endif /* _NAVEDITOR_COMPONENT_SCENE_H_ */
