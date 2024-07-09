/* ======================================================================== */
/* hardware.h                                                               */
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
#ifndef HARDWARE_DEVICE_H
#define HARDWARE_DEVICE_H

#include <engine/error.h>
#include <engine/typealias.h>

#define HARDWARE_TRUE           1
#define HARDWARE_FALSE          0
#define HARDWARE_CLIENT_API     0x000000002
#define HARDWARE_STABLE_API     0x000000004 /* HINT:  HARDWARE_CLIENT_API          */
#define HARDWARE_OPENGL_API     0x000000006 /* HINT:  HARDWARE_CLIENT_API          */
#define HARDWARE_VULKAN_API     0x000000008 /* HINT:  HARDWARE_CLIENT_API          */
#define HARDWARE_DIRECT3D_API   0x000000010 /* HINT:  HARDWARE_CLIENT_API          */
#define HARDWARE_WINDOW_VISIBLE 0x000000012 /* VALUE: HARDWARE_TRUE/HARDWARE_FALSE */

typedef struct HardwareDeviceHint {
    int client_api = HARDWARE_VULKAN_API;
    int window_visible = HARDWARE_TRUE;
} HardwareDeviceHint;

class HardwareDevice {
public:
    /* ============================== */
    /*        graphics options        */
    /* ============================== */

    /* ============================ */
    /*        window options        */
    /* ============================ */
    virtual bool window_should_close() = 0;
    virtual void poll_events() = 0;

    virtual ~HardwareDevice() = 0;
};

/* create and destroy HardwareDevice object. */
void hardware_device_hint(int hint, int value);
Error hardware_device_create(int width, int height, const char *title, HardwareDevice **ppHardwareDevice);
void hardware_device_destroy(HardwareDevice *pHardwareDevice);

#endif /* HARDWARE_DEVICE_H */
