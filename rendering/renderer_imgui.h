/* ======================================================================== */
/* renderer_imgui.h                                                         */
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
#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <imgui.h>
#include "drivers/render_device.h"
#include "renderer_screen.h"
// std
#include <unordered_map>

class RendererImGui {
public:
    RendererImGui(RenderDevice *p_device);
    ~RendererImGui();

    void initialize(RendererScreen *p_render_window);

    ImTextureID create_texture(RenderDevice::Texture2D *p_texture);
    void destroy_texture(ImTextureID texture_id);

    /* begin new gui frame */
    void cmd_begin_imgui_render(VkCommandBuffer cmd_buffer);
    void cmd_end_imgui_render(VkCommandBuffer cmd_buffer);

    void cmd_begin_window(const char *title);
    void cmd_end_window();
    void cmd_begin_viewport(const char *title);
    void cmd_end_viewport();

    void cmd_draw_texture(ImTextureID texture, uint32_t width, uint32_t height);
    void cmd_drag_float(const char *label, float *v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f");
    void cmd_drag_float2(const char *label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f");
    void cmd_drag_float3(const char *label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f");
    void cmd_drag_float4(const char *label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f");

    void cmd_show_cursor();
    void cmd_hide_cursor();

private:
    void _drag_scalar_n(const char *label, float *v, int v_number, float v_speed, float v_min, float v_max, const char *format);
    void _check_dragging_cursor(); // check the drag cursor is need show or hide.
    void _set_theme_embrace_the_darkness();

    RenderDevice *rd;
    RendererScreen *screen;
};

#endif /* _EDITOR_H_ */
