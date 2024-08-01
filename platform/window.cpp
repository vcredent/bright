/* ======================================================================== */
/* window.cpp                                                               */
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
#include "window.h"

/* record window create instance count */
static int window_instance_count = 0;

Window::Window(const char *title, int width, int height)
{
    if (window_instance_count <= 0) {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    handle = glfwCreateWindow(width, height, title, NULL, NULL);
    EXIT_FAIL_COND_V(handle, "-copilot engine error: create glfw window failed.");

    glfwSetWindowUserPointer(handle, this);

    ++window_instance_count;
}

Window::~Window()
{
    glfwDestroyWindow(handle);
    --window_instance_count;
    if (window_instance_count <= 0) {
        glfwTerminate();
    }
}

void Window::get_size(Rect2D *p_rect)
{
    glfwGetWindowSize(handle, &p_rect->w, &p_rect->h);
}

int Window::getkey(int key)
{
    return glfwGetKey(handle, key);
}

int Window::get_mouse_button(int button)
{
    return glfwGetMouseButton(handle, button);
}

void Window::get_cursor_position(float *p_xpos, float *p_ypos)
{
    double x, y;
    glfwGetCursorPos(handle, &x, &y);
    *p_xpos = (float) x;
    *p_ypos = (float) y;
}


void Window::set_visible(bool is_visible)
{
    visible_flag = is_visible;

    if (is_visible) {
        glfwShowWindow(handle);
    } else {
        glfwHideWindow(handle);
    }
}

void Window::set_cursor_position(float x, float y)
{
    glfwSetCursorPos(handle, x, y);
}

void Window::set_window_close_callbacks(PFN_WindowCloseCallback callback)
{
    fnWindowCloseCallback = callback;
    glfwSetWindowCloseCallback(handle, [](GLFWwindow *glfw_window) {
        Window *window = (Window *) glfwGetWindowUserPointer(glfw_window);
        window->fnWindowCloseCallback(window);
    });
}

void Window::set_window_resize_callbacks(PFN_WindowResizeCallback callback)
{
    fnWindowResizeCallback = callback;
    glfwSetWindowSizeCallback(handle, [] (GLFWwindow *glfw_window, int w, int h) {
        Window *window = (Window *) glfwGetWindowUserPointer(glfw_window);
        window->fnWindowResizeCallback(window, w, h);
    });
}

void Window::set_window_mouse_button_callbacks(PFN_WindowMouseButtonCallback callback)
{
    fnWindowMouseButtonCallback = callback;
    glfwSetMouseButtonCallback(handle, [](GLFWwindow *glfw_window, int button, int action, int mods) {
        Window *window = (Window *) glfwGetWindowUserPointer(glfw_window);
        window->fnWindowMouseButtonCallback(window, button, action, mods);
    });
}

void Window::set_window_cursor_position_callbacks(PFN_WindowCursorPositionCallback callback)
{
    fnWindowCursorPositionCallback = callback;
    glfwSetCursorPosCallback(handle, [](GLFWwindow *glfw_window, double x, double y) {
        Window *window = (Window *) glfwGetWindowUserPointer(glfw_window);
        window->fnWindowCursorPositionCallback(window, x, y);
    });
}

void Window::set_window_key_callbacks(PFN_WindowKeyCallback callback)
{
    fnWindowKeyCallback = callback;
    glfwSetKeyCallback(handle, [](GLFWwindow *glfw_window, int key, int scancode, int action, int mods) {
        Window *window = (Window *) glfwGetWindowUserPointer(glfw_window);
        window->fnWindowKeyCallback(window, key, scancode, action, mods);
    });
}

bool Window::is_close()
{
    return !glfwWindowShouldClose(handle);
}

void Window::poll_events()
{
    glfwMakeContextCurrent(handle);
    glfwPollEvents();
}

void Window::toggle_full_screen()
{
    full_screen_flag = !full_screen_flag;

    if (full_screen_flag) {
        GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *vidmode = glfwGetVideoMode(primary_monitor);

        glfwGetWindowPos(handle, &full_screen_rect.x, &full_screen_rect.y);
        get_size(&full_screen_rect);

        glfwSetWindowMonitor(handle, primary_monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
        return;
    }

    glfwSetWindowMonitor(handle, NULL, full_screen_rect.x, full_screen_rect.y, full_screen_rect.w, full_screen_rect.h, 0);
}

void Window::show_cursor()
{
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::hide_cursor()
{
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}