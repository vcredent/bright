/* ======================================================================== */
/* renderer_scene.h                                                         */
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
#include "renderer_scene.h"

RendererScene::RendererScene(RenderDevice *v_rd)
{
    rd = v_rd;

    render_data = memnew(SceneRenderData, rd);


    directional_light = memnew(RenderingDirectionalLight, rd);

    scene = memnew(RenderingScene, rd);
    scene->initialize();

    axisline = memnew(RenderingCoordinateAxis, rd, render_data);
    axisline->initialize(scene->get_render_pass());

    graphics = memnew(RenderingGraphics, rd, render_data);
    graphics->initialize(scene->get_render_pass());
}

RendererScene::~RendererScene()
{
    memdel(axisline);
    memdel(graphics);
    memdel(directional_light);
    memdel(scene);
    memdel(render_data);
}

void RendererScene::set_scene_camera(Camera *v_camera)
{
    camera = v_camera;
}

Camera *RendererScene::get_scene_camera()
{
    return camera;
}

void RendererScene::enable_coordinate_axis(bool is_enable)
{
    show_coordinate_axis = is_enable;
}

void RendererScene::list_render_object(std::vector<RenderObject *> **p_objects)
{
    graphics->list_render_object(p_objects);
}

void RendererScene::push_render_object(RenderObject *v_object)
{
    graphics->push_render_object(v_object);
}

void RendererScene::cmd_begin_scene_renderer(uint32_t v_width, uint32_t v_height)
{
    // update
    camera->update();

    SceneRenderData::DirectionalLight light;
    directional_light->copy_data(&light);
    
    SceneRenderData::Perspective perspective;
    perspective.camera_pos = Vec4(camera->get_position(), 1.0f);
    perspective.view = camera->get_view_matrix();
    perspective.projection = camera->get_projection_matrix();

    render_data->set_render_data(
        v_width,
        v_height,
        &perspective,
        &light
    );

    // render
    scene->set_scene_extent(v_width, v_height);
    scene->cmd_begin_scene_rendering(&scene_cmd_buffer);

    if (show_coordinate_axis)
        axisline->cmd_draw_coordinate_axis(scene_cmd_buffer);
    graphics->cmd_draw_object_list(scene_cmd_buffer);
}

void RendererScene::cmd_end_scene_renderer(RenderDevice::Texture2D **scene_texture, RenderDevice::Texture2D **scene_depth)
{
    scene->cmd_end_scene_rendering();

    if (scene_texture != NULL)
        *scene_texture = scene->get_scene_texture();

    if (scene_depth != NULL)
        *scene_depth = scene->get_scene_depth();
}