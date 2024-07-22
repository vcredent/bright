/* ======================================================================== */
/* editor_render_device.h                                                   */
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
#ifndef _IMGUI_CONTEXT_H_
#define _IMGUI_CONTEXT_H_

#include <imgui.h>
#include "drivers/render_device.h"

class EditorRenderDevice {
public:
    EditorRenderDevice(RenderDevice *p_device);
    ~EditorRenderDevice();

    void initialize();

    ImTextureID create_texture_id(RenderDevice::Texture2D *p_texture);
    void destroy_texture_id(ImTextureID texture_id);

    /* begin new gui frame */
    void command_begin_new_frame(VkCommandBuffer command_buffer);
    void command_end_new_frame(VkCommandBuffer command_buffer);

    void command_begin_viewport(const char *title);
    void command_end_viewport();
    void command_draw_texture(ImTextureID texture, uint32_t *p_width, uint32_t *p_height);

private:
    void _set_theme_embrace_the_darkness();

    RenderDevice *rd;
};

#endif /* _IMGUI_CONTEXT_H_ */
