/* ======================================================================== */
/* hardware.cpp                                                             */
/* ======================================================================== */
/*                        This file is part of:                             */
/*                           PORTABLE ENGINE                                */
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
#include "hardware.h"
#include "window/vulkan_context_win32.h"

struct HardwareDeviceHint _hint;

HardwareDevice::~HardwareDevice()
{
    // ...
}

void hardware_device_hint(int hint, int value)
{
    switch (hint) {
        case HARDWARE_CLIENT_API:
            _hint.client_api = value;
            break;
        case HARDWARE_WINDOW_VISIBLE:
            _hint.window_visible = value;
            break;
        default:
            EXIT_FAIL("-engine error: unknown hint: %d", hint);
    }
}

Error hardware_device_create(int width, int height, const char *title, HardwareDevice **p_hardware)
{
    VulkanContextWin32 *vulkan_context_win32 = new VulkanContextWin32();
    vulkan_context_win32->window_create(width, height, title, &_hint);
    *p_hardware = vulkan_context_win32;
    return OK;
}

void hardware_device_destroy(HardwareDevice *hardware)
{
    delete hardware;
}