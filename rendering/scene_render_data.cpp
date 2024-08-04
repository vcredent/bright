/* ======================================================================== */
/* scene_render_data.cpp                                                    */
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
#include "scene_render_data.h"

SceneRenderData::SceneRenderData(RenderDevice *v_rd)
    : rd(v_rd)
{
    buffer = rd->create_buffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UniformBuffer));
}

SceneRenderData::~SceneRenderData()
{
    rd->destroy_buffer(buffer);
}

void SceneRenderData::set_render_data(uint32_t v_width,
                                      uint32_t v_height,
                                      const Mat4 &projection,
                                      const Mat4 &view)
{
    width = v_width;
    height = v_height;
    uniform.projection = projection;
    uniform.view = view;

    rd->write_buffer(buffer, 0, sizeof(UniformBuffer), &uniform);
}