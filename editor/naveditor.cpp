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
// components
#include "components/debugger.h"
#include "components/camera.h"
#include "components/scene.h"
#include "components/settings.h"

namespace debugger {
    DebuggerProperties *_debugger_properties =
            (DebuggerProperties *) imalloc(sizeof(DebuggerProperties));
}

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
            // engine settings
            if (MENU_ITEM("引擎设置"))
                mger.enable_engine_settings = true;
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();

    // draw main editor
    _draw_main_editor();
}

void Naveditor::cmd_end_naveditor_render(VkCommandBuffer cmd_buffer)
{
    NavUI::EndNewFrame(cmd_buffer);
}

void Naveditor::cmd_draw_debugger_editor_ui()
{
    _draw_debugger_editor_ui(debugger::_debugger_properties);
}

void Naveditor::cmd_draw_camera_editor_ui(Camera *v_camera)
{
    _draw_camera_editor_ui(v_camera);
}

void Naveditor::cmd_draw_scene_viewport_ui(RenderDevice::Texture2D *v_texture, RenderDevice::Texture2D *v_depth, ImVec2 *p_region)
{
    _draw_scene_editor_ui(v_texture, v_depth, p_region);
}

void Naveditor::_draw_main_editor()
{
    if (mger.enable_engine_settings)
        _draw_engine_settings_editor_ui(&mger.enable_engine_settings);
}