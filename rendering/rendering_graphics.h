/* ======================================================================== */
/* rendering_graphics.h                                                     */
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
#ifndef _RENDERER_GRAPHICS_H_
#define _RENDERER_GRAPHICS_H_

#include "render_object.h"

class RenderingGraphics {
public:
    U_MEMNEW_ONLY RenderingGraphics(RenderDevice *v_rd);
    ~RenderingGraphics();

    void initialize(VkRenderPass render_pass);
    void push_render_object(RenderObject *object);

    void cmd_begin_graphics_render(VkCommandBuffer cmd_buffer);
    void cmd_end_graphics_render(VkCommandBuffer cmd_buffer);

    void cmd_setval_viewport(VkCommandBuffer cmd_buffer, uint32_t w, uint32_t h);
    void cmd_setval_view_matrix(VkCommandBuffer cmd_buffer, Mat4 view);
    void cmd_setval_projection_matrix(VkCommandBuffer cmd_buffer, Mat4 projection);
    void cmd_setval_model_matrix(VkCommandBuffer cmd_buffer, Mat4 model);
    void cmd_draw_list(VkCommandBuffer cmd_buffer);

private:
    RenderDevice *rd;

    struct Transform {
        Mat4 m;
        Mat4 v;
        Mat4 p;
    } transform;

    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorSet descriptor_set;
    RenderDevice::Buffer *transform_buffer;
    RenderDevice::Pipeline *pipeline;

    std::vector<RenderObject *> render_objects;
};

#endif /* _RENDERER_GRAPHICS_H_ */
