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
#include "rendering/renderer.h"
#include <copilot/typedefs.h>

// components
#include "components/debugger.h"
#include "components/scene.h"
#include "components/settings.h"
#include "components/scene_node_browser.h"

#include <stb/stb_image.h>

namespace Debugger {
    DebuggerProperties *v_debugger_properties =
            (DebuggerProperties *) imalloc(sizeof(DebuggerProperties));
}

#define MENU_ITEM(title) ImGui::MenuItem("        " title)

Naveditor::Naveditor(RenderDevice *v_rd, RenderingScreen *v_screen)
    : rd(v_rd)
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

    _initialize_icon();
}

Naveditor::~Naveditor()
{
    for (const auto &item: icons) {
        NavUI::RemoveTexture(item.second->texture);
        rd->destroy_texture(item.second->image);
        free(item.second);
    }

    rd->destroy_sampler(sampler);
    NavUI::Destroy();
}

void Naveditor::cmd_begin_naveditor_render(VkCommandBuffer cmd_buffer)
{
    NavUI::BeginNewFrame(cmd_buffer);
    _check_values();

    // menu bar
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15.0f, 5.0f));
    ImGui::BeginMainMenuBar();
    {
        ImGui::PopStyleVar(2);
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
    _draw_debugger_editor_ui(Debugger::v_debugger_properties);
}

void Naveditor::cmd_draw_scene_viewport_ui(RenderDevice::Texture2D *v_texture, RenderDevice::Texture2D *v_depth, ImVec2 *p_region)
{
    _draw_scene_editor_ui(v_texture, v_depth, p_region);
}

void Naveditor::cmd_draw_scene_node_browser()
{
    std::vector<RenderObject *> *objects;
    Renderer::list_render_object(&objects);

    Camera *camera;
    Renderer::get_scene_camera(&camera);

    std::vector<ClassProperties *> properties;
    properties.push_back(camera);
    for (const auto &item: *objects)
        properties.push_back(item);

    _cmd_draw_scene_node_browser(properties, this);
}

void Naveditor::_initialize_icon()
{
    unsigned char* pixels;
    int width, height, channels;

    rd->create_sampler(&sampler);

    // cube.png

    Navicon *cube = (Navicon *) imalloc(sizeof(Navicon));
    cube->name = "cube";
    pixels = stbi_load(_CURDIR("resource/icon/cube.png"), &width, &height, &channels, STBI_rgb_alpha);
    cube->image = rd->create_texture(width, height, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM , VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    rd->bind_texture_sampler(cube->image, sampler);
    rd->write_texture(cube->image, pixels);
    cube->texture = NavUI::AddTexture(cube->image->sampler, cube->image->image_view, cube->image->image_layout);
    icons[cube->name] = cube;
    stbi_image_free(pixels);

    // camera.png
    Navicon *camera = (Navicon *) imalloc(sizeof(Navicon));
    camera->name = "camera";
    pixels = stbi_load(_CURDIR("resource/icon/camera.png"), &width, &height, &channels, STBI_rgb_alpha);
    camera->image = rd->create_texture(width, height, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM , VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    rd->bind_texture_sampler(camera->image, sampler);
    rd->write_texture(camera->image, pixels);
    camera->texture = NavUI::AddTexture(camera->image->sampler, camera->image->image_view, camera->image->image_layout);
    icons[camera->name] = camera;
    stbi_image_free(pixels);
}

void Naveditor::_check_values()
{
    Renderer::enable_coordinate_axis(setting_values.render_show_coordinate);

    if (setting_values.imgui_show_demo_window)
        ImGui::ShowDemoWindow(&setting_values.imgui_show_demo_window);
}

void Naveditor::_draw_main_editor()
{
    if (mger.enable_engine_settings)
        _draw_engine_settings_editor_ui(&mger.enable_engine_settings, &setting_values);
}