/* ======================================================================== */
/* window.cpp                                                               */
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
#include <RT/Window/window.h>

/* record window create instance count */
static int GLOB_WindowInstanceCount = 0;

Window::Window(const char *title, int width, int height)
{
    if (GLOB_WindowInstanceCount <= 0) {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    handle = glfwCreateWindow(width, height, title, NULL, NULL);
    EXIT_FAIL_COND_V(handle, "-bright engine error: create glfw window failed.");

    glfwSetWindowUserPointer(handle, this);

    ++GLOB_WindowInstanceCount;
}

Window::~Window()
{
    glfwDestroyWindow(handle);
    --GLOB_WindowInstanceCount;
    if (GLOB_WindowInstanceCount <= 0) {
        glfwTerminate();
    }
}

void Window::GetSize(Rect2D *pRect)
{
    glfwGetWindowSize(handle, &pRect->w, &pRect->h);
}

int Window::GetKey(int key)
{
    return glfwGetKey(handle, key);
}

int Window::GetMouseButton(int button)
{
    return glfwGetMouseButton(handle, button);
}

void Window::GetCursorPosition(float* xpos, float* ypos)
{
    double x, y;
    glfwGetCursorPos(handle, &x, &y);
    *xpos = (float) x;
    *ypos = (float) y;
}


void Window::SetVisible(bool isVisible)
{
    visibleFlag = isVisible;

    if (isVisible) {
        glfwShowWindow(handle);
    } else {
        glfwHideWindow(handle);
    }
}

void Window::SetCursorPosition(float x, float y)
{
    glfwSetCursorPos(handle, x, y);
}

void Window::SetWindowCloseCallback(PFN_WindowCloseCallback callback)
{
    fnWindowCloseCallback = callback;
    glfwSetWindowCloseCallback(handle, [](GLFWwindow *glfw_window) {
        Window *window = (Window *) glfwGetWindowUserPointer(glfw_window);
        window->fnWindowCloseCallback(window);
    });
}

void Window::SetWindowResizeCallback(PFN_WindowResizeCallback callback)
{
    fnWindowResizeCallback = callback;
    glfwSetWindowSizeCallback(handle, [] (GLFWwindow *glfw_window, int w, int h) {
        Window *window = (Window *) glfwGetWindowUserPointer(glfw_window);
        window->fnWindowResizeCallback(window, w, h);
    });
}

void Window::SetWindowMouseButtonCallback(PFN_WindowMouseButtonCallback callback)
{
    fnWindowMouseButtonCallback = callback;
    glfwSetMouseButtonCallback(handle, [](GLFWwindow *glfw_window, int button, int action, int mods) {
        Window *window = (Window *) glfwGetWindowUserPointer(glfw_window);
        window->fnWindowMouseButtonCallback(window, button, action, mods);
    });
}

void Window::SetWindowCursorPositionCallback(PFN_WindowCursorPositionCallback callback)
{
    fnWindowCursorPositionCallback = callback;
    glfwSetCursorPosCallback(handle, [](GLFWwindow *glfw_window, double x, double y) {
        Window *window = (Window *) glfwGetWindowUserPointer(glfw_window);
        window->fnWindowCursorPositionCallback(window, x, y);
    });
}

void Window::SetWindowKeyCallback(PFN_WindowKeyCallback callback)
{
    fnWindowKeyCallback = callback;
    glfwSetKeyCallback(handle, [](GLFWwindow *glfw_window, int key, int scancode, int action, int mods) {
        Window *window = (Window *) glfwGetWindowUserPointer(glfw_window);
        window->fnWindowKeyCallback(window, key, scancode, action, mods);
    });
}

void Window::SetWindowScrollCallback(PFN_WindowScrollCallback callback)
{
    fnWindowScrollCallback = callback;
    glfwSetScrollCallback(handle, [](GLFWwindow *glfw_window, double x, double y) {
        Window *window = (Window *) glfwGetWindowUserPointer(glfw_window);
        window->fnWindowScrollCallback(window, (float) x, (float) y);
    });
}


bool Window::IsClose()
{
    return glfwWindowShouldClose(handle);
}

void Window::PollEvents()
{
    glfwMakeContextCurrent(handle);
    glfwPollEvents();
}

void Window::ToggleFullScreen()
{
    fullScreenFlag = !fullScreenFlag;

    if (fullScreenFlag) {
        GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *vidmode = glfwGetVideoMode(primaryMonitor);

        glfwGetWindowPos(handle, &fullScreenRect.x, &fullScreenRect.y);
        GetSize(&fullScreenRect);

        glfwSetWindowMonitor(handle, primaryMonitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
        return;
    }

    glfwSetWindowMonitor(handle, NULL, fullScreenRect.x, fullScreenRect.y, fullScreenRect.w, fullScreenRect.h, 0);
}

void Window::ShowCursor()
{
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::HideCursor()
{
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}