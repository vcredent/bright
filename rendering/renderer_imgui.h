/* ======================================================================== */
/* renderer_imgui.h                                                         */
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
#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <imgui.h>
#include "drivers/render_device.h"
#include "renderer_screen.h"
// std
#include <unordered_map>

class RegisterEventCallback;

typedef void (*PFN_RegisterEventWindowCloseCallback) (RegisterEventCallback *event);
typedef void (*PFN_RegisterEventWindowResizeCallback) (RegisterEventCallback *event, int w, int h);
typedef void (*PFN_RegisterEventWindowMouseButtonCallback) (RegisterEventCallback *event, int button, int action, int mods);
typedef void (*PFN_RegisterEventWindowCursorPositionCallback) (RegisterEventCallback *event, float x, float y);

class RendererImGui {
public:
    RendererImGui(RenderDevice *p_device);
    ~RendererImGui();

    void initialize(RendererScreen *p_render_window);

    ImTextureID create_texture(RenderDevice::Texture2D *p_texture);
    void destroy_texture(ImTextureID texture_id);

    void register_event_window(const char *title, RegisterEventCallback *window);
    void unregister_event_window(const char *title);
    void register_window_user_pointer(const char *title, const char *name, void *pointer);
    void register_window_resize_callback(const char *title, PFN_RegisterEventWindowResizeCallback fnEventWindowResizeCallback);
    void register_window_mouse_button_callback(const char *title, PFN_RegisterEventWindowMouseButtonCallback fnEventWindowMouseButtonCallback);
    void register_window_cursor_position_callback(const char *title, PFN_RegisterEventWindowCursorPositionCallback fnEventWindowCursorPositionCallback);

    void *get_window_user_pointer(const char *title, const char *name);

    /* begin new gui frame */
    void cmd_begin_imgui_render(VkCommandBuffer cmd_buffer);
    void cmd_end_imgui_render(VkCommandBuffer cmd_buffer);

    void cmd_begin_window(const char *title);
    void cmd_end_window();
    void cmd_begin_viewport(const char *title);
    void cmd_end_viewport();

    void cmd_same_line128();
    void cmd_draw_texture(ImTextureID texture, uint32_t width, uint32_t height);
    void cmd_drag_float(const char *label, float *v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f");
    void cmd_drag_float2(const char *label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f");
    void cmd_drag_float3(const char *label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f");
    void cmd_drag_float4(const char *label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f");

    void cmd_show_cursor();
    void cmd_hide_cursor();

private:
    struct EventCallbacks {
        RegisterEventCallback *window;
        std::unordered_map<const char *, void *> pointer = stackalloc();

        struct {
            uint32_t w;
            uint32_t h;
        } region;

        struct {
            bool button[5];
            int  current_press_button = -1;
        } mouse;

        struct {
            float x;
            float y;
        } cursor;

        PFN_RegisterEventWindowResizeCallback fnRegisterEventWindowResizeCallback = NULL;
        PFN_RegisterEventWindowMouseButtonCallback fnRegisterEventWindowMouseButtonCallback = NULL;
        PFN_RegisterEventWindowCursorPositionCallback fnRegisterEventWindowCursorPositionCallback = NULL;
    };

    void _window_event_process(EventCallbacks *callbacks);
    bool _check_event_callbacks(const char *title, EventCallbacks **p_callbacks);
    void _check_dragging_cursor();
    void _set_theme_embrace_the_darkness();

    RenderDevice *rd;
    RendererScreen *screen;

    std::unordered_map<const char *, EventCallbacks> window_event_callbacks;
};

class RegisterEventCallback {
public:
    RegisterEventCallback(const char *v_title, RendererImGui *v_rd)
            : title(v_title), rd(v_rd)
      {
        rd->register_event_window(title, this);
      }

    ~RegisterEventCallback()
      {
        /* do nothing... */
      }

    RendererImGui *get_renderer()
      {
        return rd;
      }

    const char *get_title()
      {
        return title;
      }

    void add_window_user_pointer(const char *name, void *pointer)
      {
        rd->register_window_user_pointer(title, name, pointer);
      }

    template<typename T>
    T *pointer(const char *name)
      {
        return (T*) rd->get_window_user_pointer(title, name);
      }

    void set_window_resize_callback(PFN_RegisterEventWindowResizeCallback fnEventWindowResizeCallback)
      {
        rd->register_window_resize_callback(title, fnEventWindowResizeCallback);
      }

    void set_window_mouse_button_callback(PFN_RegisterEventWindowMouseButtonCallback fnEventWindowMouseButtonCallback)
      {
        rd->register_window_mouse_button_callback(title, fnEventWindowMouseButtonCallback);
      }

    void set_window_cursor_position_callback(PFN_RegisterEventWindowCursorPositionCallback fnEventWindowCursorPositionCallback)
      {
        rd->register_window_cursor_position_callback(title, fnEventWindowCursorPositionCallback);
      }

    void set_window_size(uint32_t w, uint32_t h)
      {
        width = w;
        height = h;
      }

protected:
    const char *title;
    RendererImGui *rd;
    uint32_t width  = 8;
    uint32_t height = 8;
};

#endif /* _EDITOR_H_ */
