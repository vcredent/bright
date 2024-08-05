/* ======================================================================== */
/* naveditor.cpp                                                            */
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
#include "naveditor.h"

#define MENU_ITEM(title) ImGui::MenuItem("        " title)

Naveditor::Naveditor(RenderDevice *v_rd, RenderingScreen *v_screen)
{
    RenderDeviceContext *rdc = v_rd->get_device_context();
    
    NavUI::InitializeInfo initialize_info = {};
    initialize_info.window = (GLFWwindow *) v_screen->get_focused_window()->get_native_window();
    initialize_info.Instance = rdc->get_instance();
    initialize_info.PhysicalDevice = rdc->get_physical_device();
    initialize_info.Device = rdc->get_device();
    initialize_info.QueueFamily = rdc->get_graph_queue_family();
    initialize_info.Queue = rdc->get_graph_queue();
    initialize_info.DescriptorPool = v_rd->get_descriptor_pool();
    initialize_info.RenderPass = v_screen->get_render_pass();
    initialize_info.MinImageCount = v_screen->get_image_buffer_count();
    initialize_info.ImageCount = v_screen->get_image_buffer_count();
    initialize_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    NavUI::Initialize(&initialize_info);

    debugger = &_DebuggerProperties;
}

Naveditor::~Naveditor()
{
    NavUI::Destroy();
}

void Naveditor::cmd_begin_naveditor_render(VkCommandBuffer cmd_buffer)
{
    NavUI::BeginNewFrame(cmd_buffer);

    // menu bar
    ImGui::BeginMainMenuBar();
    {
        if (ImGui::BeginMenu("文件")) {
            ImGui::Separator();
            // scene settings
            if (MENU_ITEM("场景设置"))
                mger.enable_scene_settings = true;
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();

    // draw main editor
    _draw_main_editor();
}

void Naveditor::cmd_end_naveditor_render(VkCommandBuffer cmd_buffer)
{
    fps_list.push_back(debugger->fps);
    if (std::size(fps_list) > 255)
        fps_list.erase(fps_list.begin());

    NavUI::EndNewFrame(cmd_buffer);
}

void Naveditor::cmd_draw_debugger_editor_ui()
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
        ImGui::PlotLines("##", std::data(fps_list), std::size(fps_list), 0, NULL, 0.0f, 144.0f, ImVec2(0.0f, 32.0f));
        ImGui::Unindent(12.0f);
        ImGui::Unindent(32.0f);
    }
    NavUI::End();
}

void Naveditor::cmd_draw_camera_editor_ui(Camera *v_camera)
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

void Naveditor::cmd_draw_scene_viewport_ui(RenderDevice::Texture2D *v_texture, RenderDevice::Texture2D *v_depth, ImVec2 *p_region)
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

void Naveditor::_draw_main_editor()
{

}