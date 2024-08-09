/* ======================================================================== */
/* window.h                                                                 */
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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied  */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#ifndef _BRIGHT_WINDOW_H_
#define _BRIGHT_WINDOW_H_

#include <GLFW/glfw3.h>
#include <Bright/Error.h>
#include <unordered_map>

class Window;

typedef void (*PFN_WindowCloseCallback) (Window *window);
typedef void (*PFN_WindowResizeCallback) (Window *window, int w, int h);
typedef void (*PFN_WindowMouseButtonCallback) (Window *window, int button, int action, int mods);
typedef void (*PFN_WindowCursorPositionCallback) (Window *window, float x, float y);
typedef void (*PFN_WindowKeyCallback) (Window *window, int key, int scancode, int action, int mods);
typedef void (*PFN_WindowScrollCallback) (Window *window, float x, float y);

struct Rect2D {
    int w;
    int h;
    int x;
    int y;
};

class Window {
public:
    Window(const char *title, int width, int height);
    ~Window();

#if defined(VK_VERSION_1_0)
    void create_vulkan_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR *p_surface)
      {
          glfwCreateWindowSurface(instance, handle, allocator, p_surface);
      }
#endif

    template<typename T>
    T *pointer(const std::string& name)
      {
        void *retval = nullptr;

        auto search = pointers.find(name);
        if (search != pointers.end())
            retval = search->second;

        return (T*) retval;
      }

    void add_user_pointer(const std::string& name, void *pointer)
      {
          pointers.insert({ name, pointer });
      }

    void remove_user_pointer(const std::string& name)
      {
        pointers.erase(name);
      }

    void get_size(Rect2D *p_rect);
    void *get_native_window() { return handle; }
    int getkey(int key);
    int get_mouse_button(int button);
    void get_cursor_position(float *p_xpos, float *p_ypos);

    void set_visible(bool is_visible);
    void set_cursor_position(float x, float y);

    void set_window_close_callbacks(PFN_WindowCloseCallback callback);
    void set_window_resize_callbacks(PFN_WindowResizeCallback callback);
    void set_window_mouse_button_callbacks(PFN_WindowMouseButtonCallback callback);
    void set_window_cursor_position_callbacks(PFN_WindowCursorPositionCallback callback);
    void set_window_key_callbacks(PFN_WindowKeyCallback callback);
    void set_window_scroll_callbacks(PFN_WindowScrollCallback callback);

    bool is_close();
    bool is_visible() { return visible_flag; }

    void poll_events();
    void toggle_full_screen();

    void show_cursor();
    void hide_cursor();

private:
    GLFWwindow *handle;
    bool visible_flag = true;
    bool full_screen_flag = false;
    Rect2D full_screen_rect;
    std::unordered_map<std::string, void *> pointers;

    PFN_WindowCloseCallback fnWindowCloseCallback = NULL;
    PFN_WindowResizeCallback fnWindowResizeCallback = NULL;
    PFN_WindowMouseButtonCallback fnWindowMouseButtonCallback = NULL;
    PFN_WindowCursorPositionCallback fnWindowCursorPositionCallback = NULL;
    PFN_WindowKeyCallback fnWindowKeyCallback = NULL;
    PFN_WindowScrollCallback fnWindowScrollCallback = NULL;
};

#endif /* _BRIGHT_WINDOW_H_ */
