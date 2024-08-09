/* ======================================================================== */
/* renderer_scene.h                                                              */
/* ======================================================================== */
/*                        This file is part of:                             */
/*                            BRIGHT ENGINE                                 */
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
#ifndef _RENDERER_SCENE_H_
#define _RENDERER_SCENE_H_

#include "drivers/render_device.h"
#include "camera/camera.h"
#include "rendering_scene.h"
#include "rendering_directional_light.h"
#include "rendering_coordinate_axis.h"
#include "rendering_graphics.h"
#include "rendering_sky_sphere.h"

class RendererScene {
public:
    U_MEMNEW_ONLY RendererScene(RenderDevice *v_rd);
   ~RendererScene();

    // api
    void set_scene_camera(Camera *v_camera);
    Camera *get_scene_camera();
    RenderingDirectionalLight* get_directional_light() { return directional_light; }
    RenderingSkySphere* get_sky_shpere() { return skysphere; }
    void enable_coordinate_axis(bool is_enable);
    void list_render_object(std::vector<RenderObject *> **p_objects);
    void push_render_object(RenderObject *v_object);
    void cmd_begin_scene_renderer(uint32_t v_width, uint32_t v_height);
    void cmd_end_scene_renderer(RenderDevice::Texture2D **scene_texture, RenderDevice::Texture2D **scene_depth);

private:
    RenderDevice *rd;
    SceneRenderData *render_data;
    RenderingScene *scene;
    RenderingCoordinateAxis *axisline;
    RenderingSkySphere *skysphere;
    RenderingDirectionalLight* directional_light;
    RenderingGraphics *graphics;
    VkCommandBuffer scene_cmd_buffer;
    Camera *camera;

    bool show_coordinate_axis = true;
};

#endif /* _RENDERER_SCENE_H_ */
