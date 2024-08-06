/* ======================================================================== */
/* renderer.h                                                               */
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
#ifndef _ENGINE_RENDERER_API_H_
#define _ENGINE_RENDERER_API_H_

#include "drivers/render_device.h"
#include "camera/camera.h"
// renderer
#include "renderer_scene.h"

class Renderer {
public:
    // initialize and destroy
    static void initialize(RenderDevice *v_rd);
    static void destroy();

    // api
    static void enable_coordinate_axis(bool is_enable);
    static void list_render_object(std::vector<RenderObject *> **p_objects);
    static void push_render_object(RenderObject *v_object);

    static void begin_scene(Camera *v_camera, uint32_t v_width, uint32_t v_height);
    static void end_scene(RenderDevice::Texture2D **texture, RenderDevice::Texture2D **depth);

private:
    static RenderDevice *rd;
    static RendererScene *scene;
};

#endif /* _ENGINE_RENDERER_API_H_ */
