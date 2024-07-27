/* ======================================================================== */
/* renderer_viewport.h                                                      */
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
#ifndef _RENDERER_VIEWPORT_H_
#define _RENDERER_VIEWPORT_H_

#include "render/renderer_imgui.h"

class RendererViewport : public RegisterEventCallback {
public:
    RendererViewport(const char *v_title, RendererImGui *v_rd);
    ~RendererViewport();

    void cmd_begin_viewport_render();
    void cmd_end_viewport_render();
    void cmd_draw_image(RenderDevice::Texture2D *v_texture);

private:
    ImTextureID texture = NULL;
};

#endif /* _RENDERER_VIEWPORT_H_ */
