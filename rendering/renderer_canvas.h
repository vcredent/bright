/* ======================================================================== */
/* renderer_canvas.h                                                        */
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
#ifndef _CONVAS_H_
#define _CONVAS_H_

#include "drivers/render_device.h"

class RendererCanvas {
public:
    RendererCanvas(RenderDevice *p_device);
   ~RendererCanvas();

    void initialize();

    void set_extent(uint32_t v_width, uint32_t v_height);
    uint32_t get_width() { return width; }
    uint32_t get_height() { return height; }
    VkRenderPass get_render_pass() { return render_pass; }

    void cmd_begin_canvas_render(VkCommandBuffer *p_cmd_buffer);
    RenderDevice::Texture2D *cmd_end_canvas_render();

private:
    void _create_canvas_texture(uint32_t width, uint32_t height);
    void _clean_up_canvas_texture();

    RenderDevice *rd;
    RenderDeviceContext *rdc;
    VkRenderPass render_pass;
    RenderDevice::Texture2D *texture = NULL;
    RenderDevice::Texture2D *depth = NULL;
    RenderDevice::Texture2D *msaa = NULL;
    VkFramebuffer framebuffer;
    VkSampler sampler;
    VkCommandBuffer canvas_cmd_buffer;
    VkQueue graph_queue;
    VkFormat depth_format;

    uint32_t width = 32;
    uint32_t height = 32;
};

#endif /* _CONVAS_H_ */
