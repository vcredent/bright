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
#include "editor/camera_editor_ui.h"
#include "editor/debugger_editor_ui.h"
#include "editor/scene_editor_ui.h"

Window *window;
RenderDeviceContext *rdc;
RenderDevice *rd;
RenderingScreen *screen;
RenderObject *object;
ProjectionCamera *camera;
CameraController *game_player_controller;
RenderDevice::Texture2D *scene_preview_texture;
RenderDevice::Texture2D *scene_depth_texture;
ImVec2 scene_region = ImVec2(32.0f, 32.0f);
FPSCounter fps_counter;

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
}

void update()
{
    _update_camera();
}

void rendering()
{
    /* render to scene */
    double scene_render_start_time = glfwGetTime();
    Renderer::BeginScene(camera, scene_region.x, scene_region.y);
    Renderer::EndScene(&scene_preview_texture, &scene_depth_texture);
    double scene_render_end_time = glfwGetTime();

    double screen_render_start_time = glfwGetTime();
    VkCommandBuffer screen_cmd_buffer;
    screen->cmd_begin_screen_render(&screen_cmd_buffer);
    {
        /* ImGui */
        NavUI::BeginNewFrame(screen_cmd_buffer);
        {
            static bool show_demo_flag = true;
            ImGui::ShowDemoWindow(&show_demo_flag);

            naveditor::draw_scene_editor_ui(scene_preview_texture, scene_depth_texture, &scene_region);

            ImGui::Begin("object");
            {
                ImGui::SeparatorText("变换");
                Vec3 position = object->get_object_position();
                NavUI::DragFloat3("平移: ", glm::value_ptr(position), 0.01f);
                object->set_object_position(position);

                Vec3 rotation = object->get_object_rotation();
                NavUI::DragFloat3("旋转: ", glm::value_ptr(rotation), 0.01f);
                object->set_object_rotation(rotation);

                Vec3 scaling = object->get_object_scaling();
                NavUI::DragFloat3("缩放: ", glm::value_ptr(scaling), 0.01f);
                object->set_object_scaling(scaling);
            }
            ImGui::End();

            naveditor::draw_debugger_editor_ui();
            naveditor::draw_camera_editor_ui(camera);
        }
        NavUI::EndNewFrame(screen_cmd_buffer);
    }
    screen->cmd_end_screen_render(screen_cmd_buffer);
    double screen_render_end_time = glfwGetTime();

    // debug
    naveditor::debugger->screen_render_time = (screen_render_end_time - screen_render_start_time) * 1000.0f;
    naveditor::debugger->scene_render_time = (scene_render_end_time - scene_render_start_time) * 1000.0f;
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

    Renderer::Initialize(rd);

    object = RenderObject::load_obj("../assets/cube.obj");
    Renderer::PushSceneRenderObject(object);

    NavUI::InitializeInfo initialize_info = {};
    initialize_info.window = (GLFWwindow *) screen->get_focused_window()->get_native_window();
    initialize_info.Instance = rdc->get_instance();
    initialize_info.PhysicalDevice = rdc->get_physical_device();
    initialize_info.Device = rdc->get_device();
    initialize_info.QueueFamily = rdc->get_graph_queue_family();
    initialize_info.Queue = rdc->get_graph_queue();
    initialize_info.DescriptorPool = rd->get_descriptor_pool();
    initialize_info.RenderPass = screen->get_render_pass();
    initialize_info.MinImageCount = screen->get_image_buffer_count();
    initialize_info.ImageCount = screen->get_image_buffer_count();
    initialize_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    NavUI::Initialize(&initialize_info);

    camera = memnew(ProjectionCamera);
    game_player_controller = memnew(GamePlayerCameraController);
    game_player_controller->make_current_camera(camera);
    camera->set_position(Vec3(0.0f, 0.0f, 6.0f));
}

int main(int argc, char **argv)
{
    initialize();

    while (window->is_close()) {
        fps_counter.update();
        /* poll events */
        window->poll_events();
        update();
        rendering();
        mouse_scroll_xoffset = 0.0f;
        mouse_scroll_yoffset = 0.0f;

        naveditor::add_debugger_fps_value(fps_counter.fps());

    }

    memdel(object);
    memdel(camera);
    Renderer::Destroy();
    memdel(screen);
    NavUI::Destroy();
    ((RenderDeviceContextWin32 *) rdc)->destroy_render_device(rd);
    memdel(rdc);
    memdel(window);

    return 0;
}