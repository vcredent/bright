/* ======================================================================== */
/* renderer_imgui.cpp                                                       */
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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied  */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#include "renderer_imgui.h"

RendererImGui::RendererImGui(RenderDevice *p_device)
    : rd(p_device)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
}

RendererImGui::~RendererImGui()
{
    NavUI::Destroy();
}

void RendererImGui::initialize(RendererScreen *p_screen)
{
    screen = p_screen;

    // Setup Platform/Renderer backends
    auto rdc = rd->get_device_context();

    NavUI::InitializeInfo initialize_info = {};
    initialize_info.window = (GLFWwindow *) screen->get_focused_window()->get_native_window();
    initialize_info.Instance = rdc->get_instance();
    initialize_info.PhysicalDevice = rdc->get_physical_device();
    initialize_info.Device = rdc->get_device();
    initialize_info.QueueFamily = rdc->get_graph_queue_family();
    initialize_info.Queue = rdc->get_graph_queue();
    initialize_info.DescriptorPool = rd->get_descriptor_pool();
    initialize_info.RenderPass = screen->get_render_pass();
    initialize_info.MinImageCount = screen->get_image_buffer_count();
    initialize_info.ImageCount = screen->get_image_buffer_count();
    initialize_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    NavUI::Initialize(&initialize_info);
}

ImTextureID RendererImGui::create_texture(RenderDevice::Texture2D *p_texture)
{
    return (ImTextureID) NavUI::AddTexture(p_texture->sampler, p_texture->image_view, p_texture->image_layout);
}

void RendererImGui::destroy_texture(ImTextureID texture_id)
{
    NavUI::RemoveTexture((VkDescriptorSet) texture_id);
}

void RendererImGui::cmd_begin_imgui_render(VkCommandBuffer cmd_buffer)
{
    NavUI::BeginNewFrame(cmd_buffer);
}

void RendererImGui::cmd_end_imgui_render(VkCommandBuffer cmd_buffer)
{
    NavUI::EndNewFrame(cmd_buffer);
}

void RendererImGui::cmd_begin_window(const char *title)
{
    NavUI::Begin(title);
}

void RendererImGui::cmd_end_window()
{
    NavUI::End();
}

void RendererImGui::cmd_begin_viewport(const char *title)
{
    NavUI::BeginViewport(title);
}

void RendererImGui::cmd_end_viewport()
{
    NavUI::EndViewport();
}

void RendererImGui::cmd_draw_texture(ImTextureID texture, uint32_t width, uint32_t height)
{
    NavUI::DrawTexture(texture, NavVec2(width, height));
}

void RendererImGui::cmd_drag_float(const char *label, float *v, float v_speed, float v_min, float v_max, const char *format)
{
    NavUI::DragScalarN(label, v, 1, v_speed, v_min, v_max, format);
}

void RendererImGui::cmd_drag_float2(const char *label, float *v, float v_speed, float v_min, float v_max, const char *format)
{
    NavUI::DragScalarN(label, v, 2, v_speed, v_min, v_max, format);
}

void RendererImGui::cmd_drag_float3(const char *label, float *v, float v_speed, float v_min, float v_max, const char *format)
{
    NavUI::DragScalarN(label, v, 3, v_speed, v_min, v_max, format);
}

void RendererImGui::cmd_drag_float4(const char *label, float *v, float v_speed, float v_min, float v_max, const char *format)
{
    NavUI::DragScalarN(label, v, 4, v_speed, v_min, v_max, format);
}

void RendererImGui::cmd_show_cursor()
{
    screen->get_focused_window()->show_cursor();
}

void RendererImGui::cmd_hide_cursor()
{
    screen->get_focused_window()->hide_cursor();
}

void RendererImGui::_check_dragging_cursor()
{
    static bool     is_dragging = false;
    static ImGuiID  current_drag_item = 0x7FFFFFFF;

    ImGuiID item = ImGui::GetItemID();
    bool check_active = ImGui::IsItemActive() && ImGui::IsMouseDragging(0);

    if (item == current_drag_item && !check_active) {
        is_dragging = false;
        cmd_show_cursor();
        current_drag_item = 0x7FFFFFFF;
        goto _CHECK_DRAGGING_CURSOR_RETURN_TAG;
    }

    if (!is_dragging && check_active) {
        is_dragging = true;
        current_drag_item = item;
        cmd_hide_cursor();
        goto _CHECK_DRAGGING_CURSOR_RETURN_TAG;
    }

_CHECK_DRAGGING_CURSOR_RETURN_TAG:
    /* do nothing.... */
}
