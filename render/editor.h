/* ======================================================================== */
/* render_editor.h                                                          */
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
#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <imgui.h>
#include "drivers/render_device.h"
#include "screen.h"

class Editor {
public:
    Editor(RenderDevice *p_device);
    ~Editor();

    void initialize(Screen *p_render_window);

    ImTextureID create_texture(RenderDevice::Texture2D *p_texture);
    void destroy_texture(ImTextureID texture_id);

    /* begin new gui frame */
    void cmd_begin_editor_render(VkCommandBuffer cmd_buffer);
    void cmd_end_editor_render(VkCommandBuffer cmd_buffer);

    void cmd_begin_window(const char *title);
    void cmd_end_window();
    void cmd_begin_viewport(const char *title);
    void cmd_end_viewport();

    void cmd_draw_texture(ImTextureID texture, uint32_t *p_width, uint32_t *p_height);
    void cmd_drag_float(const char *label, float *v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f");
    void cmd_drag_float2(const char *label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f");
    void cmd_drag_float3(const char *label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f");
    void cmd_drag_float4(const char *label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f");

private:
    void _set_theme_embrace_the_darkness();

    RenderDevice *rd;
    Screen *screen;
};

#endif /* _EDITOR_H_ */
