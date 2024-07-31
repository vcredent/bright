/* ======================================================================== */
/* renderer_viewport.cpp                                                    */
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
#include "renderer_viewport.h"

RendererViewport::RendererViewport(const char *v_title, RendererImGui *v_rd)
    : RegisterEventCallback(v_title, v_rd)
{
    /* do nothing.... */
}

RendererViewport::~RendererViewport()
{
    /* do nothing.... */
}

void RendererViewport::cmd_begin_viewport_render()
{
    rd->cmd_begin_viewport(title);
}

void RendererViewport::cmd_end_viewport_render()
{
    rd->cmd_end_viewport();
}

void RendererViewport::cmd_draw_image(RenderDevice::Texture2D *v_texture)
{
    if (texture) {
        rd->destroy_texture(texture);
        texture = NULL;
    }

    // draw texture to viewport
    texture = rd->create_texture(v_texture);
    rd->cmd_draw_texture(texture, width, height);
}