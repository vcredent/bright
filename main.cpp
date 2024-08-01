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
#include "rendering/gui/renderer_viewport.h"
#include "rendering/renderer_graphics.h"

int main(int argc, char **argv)
{
    Window *window = memnew(Window, "CopilotEngine", 1980, 1080);

    auto rdc = std::make_unique<RenderDeviceContextWin32>(window);
    // initialize
    rdc->initialize();

    RenderDevice *rd;
    rd = rdc->load_render_device();

    RendererScreen *screen = memnew(RendererScreen, rd);
    screen->initialize(window);

    RendererCanvas *canvas = memnew(RendererCanvas, rd);
    canvas->initialize();

    RendererGraphics *graphics = memnew(RendererGraphics, rd);
    graphics->initialize(canvas->get_render_pass());

    RenderObject *object = RenderObject::load_assets_obj("../assets/.obj");
    graphics->push_render_object(object);

    RendererImGui *imgui = memnew(RendererImGui, rd);
    imgui->initialize(screen);

    ProjectionCamera *camera = memnew(ProjectionCamera);
    CameraController *game_player_controller = memnew(GamePlayerCameraController);
    game_player_controller->make_current_camera(camera);
    camera->set_position(Vec3(0.0f, 0.0f, 6.0f));

    RendererViewport *viewport = memnew(RendererViewport, "视口", imgui);
    viewport->add_window_user_pointer("#CANVAS", canvas);
    viewport->add_window_user_pointer("#CAMERA", camera);

    viewport->set_window_resize_callback([](RegisterEventCallback *event, int w, int h) {
        event->pointer<RendererCanvas>("#CANVAS")->set_canvas_extent(w, h);
        event->pointer<ProjectionCamera>("#CAMERA")->set_aspect_ratio((float) w / (float) h);
    });

    static bool show_demo_flag = true;

    while (window->is_close()) {
        /* poll events */
        window->poll_events();

        camera->update();

        /* render to canvas */
        VkCommandBuffer canvas_cmd_buffer;
        canvas->cmd_begin_canvas_render(&canvas_cmd_buffer);
        {
            graphics->cmd_begin_graphics_render(canvas_cmd_buffer);
            {
                graphics->cmd_setval_viewport(canvas_cmd_buffer, canvas->get_canvas_width(), canvas->get_canvas_height());
                graphics->cmd_setval_view_matrix(canvas_cmd_buffer, camera->get_view_matrix());
                graphics->cmd_setval_projection_matrix(canvas_cmd_buffer, camera->get_projection_matrix());
                graphics->cmd_draw_list(canvas_cmd_buffer);
            }
            graphics->cmd_end_graphics_render(canvas_cmd_buffer);
        }
        RenderDevice::Texture2D *canvas_texture = canvas->cmd_end_canvas_render();

        /* render to window */
        VkCommandBuffer window_cmd_buffer;
        screen->cmd_begin_screen_render(&window_cmd_buffer);
        {
            /* ImGui */
            imgui->cmd_begin_imgui_render(window_cmd_buffer);
            {
                ImGui::ShowDemoWindow(&show_demo_flag);
                viewport->cmd_begin_viewport_render();
                {
                    viewport->cmd_draw_image(canvas_texture);
                }
                viewport->cmd_end_viewport_render();

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
            imgui->cmd_end_imgui_render(window_cmd_buffer);
        }
        screen->cmd_end_screen_render(window_cmd_buffer);
    }

    memdel(object);
    memdel(graphics);
    memdel(viewport);
    memdel(camera);
    memdel(screen);
    memdel(canvas);
    memdel(imgui);
    rdc->destroy_render_device(rd);
    memdel(window);

    return 0;
}