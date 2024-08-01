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
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>

RendererImGui::RendererImGui(RenderDevice *p_device)
    : rd(p_device)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
}

RendererImGui::~RendererImGui()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplVulkan_Shutdown();
}

void RendererImGui::initialize(RendererScreen *p_screen)
{
    screen = p_screen;

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    io.ConfigViewportsNoAutoMerge = true;
    io.ConfigViewportsNoTaskBarIcon = true;

    // set default font.
    io.Fonts->AddFontFromFileTTF("../resource/fonts/Microsoft Yahei UI/Microsoft Yahei UI.ttf", 28.0f,
                                 nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    io.FontDefault = io.Fonts->Fonts.back();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // default theme.
    _set_theme_embrace_the_darkness();

    // Setup Platform/Renderer backends
    auto rdc = rd->get_device_context();
    ImGui_ImplGlfw_InitForVulkan((GLFWwindow *) screen->get_focused_window()->get_native_window(), true);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = rdc->get_instance();
    init_info.PhysicalDevice = rdc->get_physical_device();
    init_info.Device = rdc->get_device();
    init_info.QueueFamily = rdc->get_graph_queue_family();
    init_info.Queue = rdc->get_graph_queue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = rd->get_descriptor_pool();
    init_info.Subpass = 0;
    init_info.MinImageCount = screen->get_image_buffer_count();
    init_info.ImageCount = screen->get_image_buffer_count();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = VK_NULL_HANDLE;
    init_info.CheckVkResultFn = VK_NULL_HANDLE;
    ImGui_ImplVulkan_Init(&init_info, screen->get_render_pass());
}

ImTextureID RendererImGui::create_texture(RenderDevice::Texture2D *p_texture)
{
    return (ImTextureID) ImGui_ImplVulkan_AddTexture(p_texture->sampler, p_texture->image_view, p_texture->image_layout);
}

void RendererImGui::destroy_texture(ImTextureID texture_id)
{
    ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet) texture_id);
}

void RendererImGui::cmd_begin_imgui_render(VkCommandBuffer cmd_buffer)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // docking
    ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_PassthruCentralNode);
}

void RendererImGui::cmd_end_imgui_render(VkCommandBuffer cmd_buffer)
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Rendering
    ImGui::Render();
    ImDrawData* main_draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(main_draw_data, cmd_buffer);

    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void RendererImGui::cmd_begin_window(const char *title)
{
    ImGui::Begin(title);
}

void RendererImGui::cmd_end_window()
{
    ImGui::End();
}

void RendererImGui::cmd_begin_viewport(const char *title)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    cmd_begin_window(title);
}

void RendererImGui::cmd_end_viewport()
{
    cmd_end_window();
    ImGui::PopStyleVar();
}

void RendererImGui::cmd_draw_text(const char *text, ...)
{
    va_list va;
    va_start(va, text);
    ImGui::TextV(text, va);
    va_end(va);
}

void RendererImGui::cmd_draw_unformatted_text(const char *text)
{
    ImGui::TextUnformatted(text);
}

void RendererImGui::cmd_draw_texture(ImTextureID texture, uint32_t width, uint32_t height)
{
    ImGui::Image(texture, ImVec2(width, height));
}

void RendererImGui::cmd_drag_float(const char *label, float *v, float v_speed, float v_min, float v_max, const char *format)
{
    _drag_scalar_n(label, v, 1, v_speed, v_min, v_max, format);
}

void RendererImGui::cmd_drag_float2(const char *label, float *v, float v_speed, float v_min, float v_max, const char *format)
{
    _drag_scalar_n(label, v, 2, v_speed, v_min, v_max, format);
}

void RendererImGui::cmd_drag_float3(const char *label, float *v, float v_speed, float v_min, float v_max, const char *format)
{
    _drag_scalar_n(label, v, 3, v_speed, v_min, v_max, format);
}

void RendererImGui::cmd_drag_float4(const char *label, float *v, float v_speed, float v_min, float v_max, const char *format)
{
    _drag_scalar_n(label, v, 4, v_speed, v_min, v_max, format);
}

void RendererImGui::cmd_show_cursor()
{
    screen->get_focused_window()->show_cursor();
}

void RendererImGui::cmd_hide_cursor()
{
    screen->get_focused_window()->hide_cursor();
}

void RendererImGui::_drag_scalar_n(const char *label, float *v, int v_number, float v_speed, float v_min, float v_max, const char *format)
{
    assert(v_number <= 4);

    ImGui::PushID(label);
    ImGui::Indent(32.0f);
    cmd_draw_unformatted_text(label);
    ImGui::SameLine();

    switch (v_number) {
        case 1: ImGui::DragFloat("", v, v_speed, v_min, v_max, format);  break;
        case 2: ImGui::DragFloat2("", v, v_speed, v_min, v_max, format); break;
        case 3: ImGui::DragFloat3("", v, v_speed, v_min, v_max, format); break;
        case 4: ImGui::DragFloat4("", v, v_speed, v_min, v_max, format); break;
    }

    _check_dragging_cursor();
    ImGui::Unindent(32.0f);
    ImGui::PopID();
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

void RendererImGui::_set_theme_embrace_the_darkness()
{
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.58f, 0.58f, 0.58f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding                     = ImVec2(8.00f, 8.00f);
    style.FramePadding                      = ImVec2(5.00f, 2.00f);
    style.CellPadding                       = ImVec2(6.00f, 6.00f);
    style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
    style.IndentSpacing                     = 25;
    style.ScrollbarSize                     = 15;
    style.GrabMinSize                       = 16;
    style.WindowBorderSize                  = 1;
    style.ChildBorderSize                   = 1;
    style.PopupBorderSize                   = 1;
    style.FrameBorderSize                   = 1;
    style.TabBorderSize                     = 1;
    style.WindowRounding                    = 7;
    style.ChildRounding                     = 4;
    style.FrameRounding                     = 3;
    style.PopupRounding                     = 12;
    style.ScrollbarRounding                 = 9;
    style.GrabRounding                      = 3;
    style.LogSliderDeadzone                 = 4;
    style.TabRounding                       = 4;
}
