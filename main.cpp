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
#include "rendering/camera/projection_camera.h"
#include "rendering/camera/game_player_camera_controller.h"
#include "rendering/rendering_screen.h"
#include "utils/fps_counter.h"
#include "rendering/renderer.h"
// editor ui
#include "editor/naveditor.h"
// misc
#include "misc/shader_compile.h"
#include <copilot/ioutils.h>

Window *window;
RenderDeviceContext *rdc;
RenderDevice *rd;
RenderingScreen *screen;
Naveditor *naveditor;
RenderObject *cube;
ProjectionCamera *camera;
CameraController *game_player_controller;
RenderDevice::Texture2D *scene_preview_texture;
RenderDevice::Texture2D *scene_depth_texture;
ImVec2 scene_region = ImVec2(32.0f, 32.0f);
FPSCounter fps_counter;

static float mouse_scroll_xoffset = 0.0f;
static float mouse_scroll_yoffset = 0.0f;

void update()
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
        game_player_controller->on_event_key(GLFW_KEY_W, window->getkey(GLFW_KEY_W));
        game_player_controller->on_event_key(GLFW_KEY_S, window->getkey(GLFW_KEY_S));
        game_player_controller->on_event_key(GLFW_KEY_A, window->getkey(GLFW_KEY_A));
        game_player_controller->on_event_key(GLFW_KEY_D, window->getkey(GLFW_KEY_D));

        // mouse
        float xpos = 0.0f;
        float ypos = 0.0f;
        window->get_cursor_position(&xpos, &ypos);
        game_player_controller->on_event_cursor(xpos, ypos);

        // scroll
        game_player_controller->on_event_scroll(mouse_scroll_xoffset, mouse_scroll_yoffset);
    }

    camera->set_aspect_ratio(scene_region.x / scene_region.y);
    game_player_controller->on_update_camera();

    mouse_scroll_xoffset = 0.0f;
    mouse_scroll_yoffset = 0.0f;
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

    screen = memnew(RenderingScreen, rd);
    screen->initialize(window);

    naveditor = memnew(Naveditor, rd, screen);

    Renderer::initialize(rd);

    cube = RenderObject::load_obj(_CURDIR("assets/cube.obj"));
    cube->set_node_name("立方体");
    Renderer::push_render_object(cube);

    camera = memnew(ProjectionCamera);
    camera->set_node_name("场景相机");
    game_player_controller = memnew(GamePlayerCameraController);
    game_player_controller->make_current_camera(camera);
    camera->set_position(Vec3(0.0f, 0.0f, 6.0f));
    Renderer::set_scene_camera(camera);
}

int main(int argc, char **argv)
{
    compile_shader();
    initialize();

    while (window->is_close()) {
        fps_counter.update();
        /* poll events */
        window->poll_events();
        update();

        /* render to scene */
        double scene_render_start_time = glfwGetTime();
        Renderer::begin_scene(scene_region.x, scene_region.y);
        Renderer::end_scene(&scene_preview_texture, &scene_depth_texture);
        double scene_render_end_time = glfwGetTime();

        double screen_render_start_time = glfwGetTime();
        VkCommandBuffer screen_cmd_buffer;
        screen->cmd_begin_screen_render(&screen_cmd_buffer);
        {
            /* ImGui */
            naveditor->cmd_begin_naveditor_render(screen_cmd_buffer);
            {
                ImGui::Begin("Demo");
                {
                    if (ImGui::CollapsingHeader("Hello")) {
                        float v;
                        ImGui::DragFloat("view", &v);
                    }
                }
                ImGui::End();

                naveditor->cmd_draw_debugger_editor_ui();
                naveditor->cmd_draw_scene_node_browser();
                naveditor->cmd_draw_scene_viewport_ui(scene_preview_texture, scene_depth_texture, &scene_region);
            }
            naveditor->cmd_end_naveditor_render(screen_cmd_buffer);
        }
        screen->cmd_end_screen_render(screen_cmd_buffer);
        double screen_render_end_time = glfwGetTime();

        // set render debug time for debug.
        Debugger::set_scene_render_time_value((scene_render_end_time - scene_render_start_time) * 1000.0f);
        Debugger::set_screen_render_time((screen_render_end_time - screen_render_start_time) * 1000.0f);
        Debugger::set_fps_value(fps_counter.fps());
    }

    memdel(cube);
    memdel(camera);
    Renderer::destroy();
    memdel(naveditor);
    memdel(screen);
    ((RenderDeviceContextWin32 *) rdc)->destroy_render_device(rd);
    memdel(rdc);
    memdel(window);

    return 0;
}