/* ======================================================================== */
/* main.cpp                                                                 */
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
#include "platform/win32/render_device_context_win32.h"
#include <vector>
#include <chrono>
#include "rendering/camera/projection_camera.h"
#include "rendering/camera/game_player_camera_controller.h"
#include "rendering/renderer_imgui.h"
#include "rendering/renderer_canvas.h"
#include "rendering/renderer_screen.h"
#include "rendering/renderer_graphics.h"

Window *window;
RenderDeviceContext *rdc;
RenderDevice *rd;
RendererScreen *screen;
RendererCanvas *canvas;
RendererGraphics *graphics;
RenderObject *object;
RendererImGui *imgui;
ProjectionCamera *camera;
CameraController *game_player_controller;
RenderDevice::Texture2D *canvas_preview_texture;
ImVec2 viewport_window_region = ImVec2(32.0f, 32.0f);

static float mouse_scroll_xoffset = 0.0f;
static float mouse_scroll_yoffset = 0.0f;

void _update_camera()
{
    // update camera
    static bool is_dragging = false;
    if (window->getkey(GLFW_KEY_F)) {
        is_dragging = true;
        window->hide_cursor();
    }

    if (window->getkey(GLFW_KEY_ESCAPE) && is_dragging) {
        window->show_cursor();
        is_dragging = false;
        game_player_controller->uncontinual();
    }

    if (is_dragging) {
        // key button
        if (window->getkey(GLFW_KEY_W))
            game_player_controller->on_event_key(GLFW_KEY_W, GLFW_PRESS);

        if (window->getkey(GLFW_KEY_S))
            game_player_controller->on_event_key(GLFW_KEY_S, GLFW_PRESS);

        if (window->getkey(GLFW_KEY_A))
            game_player_controller->on_event_key(GLFW_KEY_A, GLFW_PRESS);

        if (window->getkey(GLFW_KEY_D))
            game_player_controller->on_event_key(GLFW_KEY_D, GLFW_PRESS);

        // mouse
        float xpos = 0.0f;
        float ypos = 0.0f;
        window->get_cursor_position(&xpos, &ypos);
        game_player_controller->on_event_cursor(xpos, ypos);

        // scroll
        game_player_controller->on_event_scroll(mouse_scroll_xoffset, mouse_scroll_yoffset);
    }

    game_player_controller->on_update_camera();
}

void update()
{
    canvas->set_extent(viewport_window_region.x, viewport_window_region.y);
    camera->set_aspect_ratio(viewport_window_region.x / viewport_window_region.y);
    _update_camera();
}

void rendering()
{
    /* render to canvas */
    VkCommandBuffer canvas_cmd_buffer;
    canvas->cmd_begin_canvas_render(&canvas_cmd_buffer);
    {
        graphics->cmd_begin_graphics_render(canvas_cmd_buffer);
        {
            graphics->cmd_setval_viewport(canvas_cmd_buffer, canvas->get_width(), canvas->get_height());
            graphics->cmd_setval_view_matrix(canvas_cmd_buffer, camera->get_view_matrix());
            graphics->cmd_setval_projection_matrix(canvas_cmd_buffer, camera->get_projection_matrix());
            graphics->cmd_draw_list(canvas_cmd_buffer);
        }
        graphics->cmd_end_graphics_render(canvas_cmd_buffer);
    }
    canvas_preview_texture = canvas->cmd_end_canvas_render();

    VkCommandBuffer screen_cmd_buffer;
    screen->cmd_begin_screen_render(&screen_cmd_buffer);
    {
        /* ImGui */
        imgui->cmd_begin_imgui_render(screen_cmd_buffer);
        {
            static bool show_demo_flag = true;
            ImGui::ShowDemoWindow(&show_demo_flag);

            imgui->cmd_begin_viewport("视口");
            {
                viewport_window_region = ImGui::GetContentRegionAvail();

                static ImTextureID preview = NULL;
                if (preview != NULL)
                    imgui->destroy_texture(preview);

                preview = imgui->create_texture(canvas_preview_texture);
                imgui->cmd_draw_texture(preview, canvas_preview_texture->width, canvas_preview_texture->height);
            }
            imgui->cmd_end_viewport();

            imgui->cmd_begin_window("测试");
            {
                static float x;
                static Vec2 vec2;
                static Vec3 vec3;
                static Vec4 vec4;

                imgui->cmd_drag_float("vec1", &x, 0.01f);
                imgui->cmd_drag_float2("vec2", glm::value_ptr(vec2), 0.01f);
                imgui->cmd_drag_float3("vec3", glm::value_ptr(vec3), 0.01f);
                imgui->cmd_drag_float4("vec4", glm::value_ptr(vec4), 0.01f);
            }
            imgui->cmd_end_window();

            imgui->cmd_begin_window("object");
            {
                Vec3 position = object->get_object_position();
                imgui->cmd_drag_float3("平移: ", glm::value_ptr(position), 0.01f);
                object->set_object_position(position);

                Vec3 rotation = object->get_object_rotation();
                imgui->cmd_drag_float3("旋转: ", glm::value_ptr(rotation), 0.01f);
                object->set_object_rotation(rotation);

                Vec3 scaling = object->get_object_scaling();
                imgui->cmd_drag_float3("缩放: ", glm::value_ptr(scaling), 0.01f);
                object->set_object_scaling(scaling);
            }
            imgui->cmd_end_window();

            imgui->cmd_begin_window("摄像机参数");
            {
                Vec3 position = camera->get_position();
                imgui->cmd_drag_float3("位置: ", glm::value_ptr(position), 0.01f);
                camera->set_position(position);

                Vec3 target = camera->get_target();
                imgui->cmd_drag_float3("目标: ", glm::value_ptr(target), 0.01f);
                camera->set_target(target);

                float fov = camera->get_fov();
                imgui->cmd_drag_float("景深: ", &fov, 0.01f);
                camera->set_fov(fov);

                float near = camera->get_near();
                imgui->cmd_drag_float("近点: ", &near, 0.01f);
                camera->set_near(near);

                float far = camera->get_far();
                imgui->cmd_drag_float("远点: ", &far, 0.01f);
                camera->set_far(far);
            }
            imgui->cmd_end_window();
        }
        imgui->cmd_end_imgui_render(screen_cmd_buffer);
    }
    screen->cmd_end_screen_render(screen_cmd_buffer);
}

void initialize()
{
    window = memnew(Window, "CopilotEngine", 1980, 1080);

    window->set_window_scroll_callbacks([](Window *window, float x, float y) {
        mouse_scroll_xoffset = x;
        mouse_scroll_yoffset = y;
    });

    rdc = memnew(RenderDeviceContextWin32, window);
    // initialize
    rdc->initialize();
    rd = ((RenderDeviceContextWin32 *) rdc)->load_render_device();

    screen = memnew(RendererScreen, rd);
    screen->initialize(window);

    canvas = memnew(RendererCanvas, rd);
    canvas->initialize();

    graphics = memnew(RendererGraphics, rd);
    graphics->initialize(canvas->get_render_pass());

    object = RenderObject::load_assets_obj("../assets/cube.obj");
    graphics->push_render_object(object);

    imgui = memnew(RendererImGui, rd);
    imgui->initialize(screen);

    camera = memnew(ProjectionCamera);
    game_player_controller = memnew(GamePlayerCameraController);
    game_player_controller->make_current_camera(camera);
    camera->set_position(Vec3(0.0f, 0.0f, 6.0f));
}

int main(int argc, char **argv)
{
    initialize();

    while (window->is_close()) {
        /* poll events */
        window->poll_events();
        update();
        rendering();
        mouse_scroll_xoffset = 0.0f;
        mouse_scroll_yoffset = 0.0f;
    }

    memdel(object);
    memdel(graphics);
    memdel(camera);
    memdel(screen);
    memdel(canvas);
    memdel(imgui);
    ((RenderDeviceContextWin32 *) rdc)->destroy_render_device(rd);
    memdel(rdc);
    memdel(window);

    return 0;
}