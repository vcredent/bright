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
    void CreateWindowSurfaceKHR(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR *p_surface)
      {
          glfwCreateWindowSurface(instance, handle, allocator, p_surface);
      }
#endif

    template<typename T>
    T *Pointer(const std::string& name)
      {
        void *retval = nullptr;

        auto search = pointers.find(name);
        if (search != pointers.end())
            retval = search->second;

        return (T*) retval;
      }

    void AddUserPointer(const std::string& name, void *pointer)
      {
          pointers.insert({ name, pointer });
      }

    void RemoveUserPointer(const std::string& name)
      {
        pointers.erase(name);
      }

    void GetSize(Rect2D *pRect);
    void *GetNativeHandle() { return handle; }
    int GetKey(int key);
    int GetMouseButton(int button);
    void GetCursorPosition(float *xpos, float *ypos);

    void SetVisible(bool isVisible);
    void SetCursorPosition(float x, float y);

    void SetWindowCloseCallback(PFN_WindowCloseCallback callback);
    void SetWindowResizeCallback(PFN_WindowResizeCallback callback);
    void SetWindowMouseButtonCallback(PFN_WindowMouseButtonCallback callback);
    void SetWindowCursorPositionCallback(PFN_WindowCursorPositionCallback callback);
    void SetWindowKeyCallback(PFN_WindowKeyCallback callback);
    void SetWindowScrollCallback(PFN_WindowScrollCallback callback);

    bool IsClose();
    bool IsVisible() { return visibleFlag; }

    void PollEvents();
    void ToggleFullScreen();

    void ShowCursor();
    void HideCursor();

private:
    GLFWwindow *handle;
    bool visibleFlag = true;
    bool fullScreenFlag = false;
    Rect2D fullScreenRect;
    std::unordered_map<std::string, void *> pointers;

    PFN_WindowCloseCallback fnWindowCloseCallback = NULL;
    PFN_WindowResizeCallback fnWindowResizeCallback = NULL;
    PFN_WindowMouseButtonCallback fnWindowMouseButtonCallback = NULL;
    PFN_WindowCursorPositionCallback fnWindowCursorPositionCallback = NULL;
    PFN_WindowKeyCallback fnWindowKeyCallback = NULL;
    PFN_WindowScrollCallback fnWindowScrollCallback = NULL;
};

#endif /* _BRIGHT_WINDOW_H_ */
