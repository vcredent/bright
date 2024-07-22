/* ======================================================================== */
/* render_canvas.h                                                          */
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
#ifndef _CONVAS_H_
#define _CONVAS_H_

#include "drivers/render_device.h"

class Canvas {
public:
    Canvas(RenderDevice *p_device);
   ~Canvas();

    void initialize();

    void command_begin_canvas_render(VkCommandBuffer *p_command_buffer, uint32_t width, uint32_t height);
    RenderDevice::Texture2D *command_end_canvas_render();

private:
    void _create_canvas_texture(uint32_t width, uint32_t height);
    void _clean_up_canvas_texture();

    RenderDevice *rd;
    VkRenderPass render_pass;
    RenderDevice::Texture2D *texture;
    VkFramebuffer framebuffer;
    VkSampler sampler;
    VkCommandBuffer canvas_command_buffer;

    VkQueue graph_queue;
};

#endif /* _CONVAS_H_ */
