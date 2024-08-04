/* ======================================================================== */
/* rendering_axis_line.h                                                    */
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
#ifndef _RENDERER_AXIS_LINE_H_
#define _RENDERER_AXIS_LINE_H_

#include "drivers/render_device.h"
#include "scene_render_data.h"

class RenderingAxisLine {
public:
    U_MEMNEW_ONLY RenderingAxisLine(RenderDevice *v_rd, SceneRenderData *v_render_data);
    ~RenderingAxisLine();

    void initialize(VkRenderPass render_pass);

    void cmd_draw_line(VkCommandBuffer cmd_buffer);

private:
    RenderDevice *rd;
    SceneRenderData *render_data;
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorSet descriptor_set;
    RenderDevice::Pipeline *pipeline;
};

#endif /* _RENDERER_AXIS_LINE_H_ */
