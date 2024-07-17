/* ======================================================================== */
/* vulkan_utils.h                                                           */
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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, e1ither express or implied */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#ifndef _VULKAN_UTILS_H
#define _VULKAN_UTILS_H

#include <vulkan/vulkan.h>
#include <fstream>
#include <stdexcept>
#include <copilot/memalloc.h>
#include <copilot/ioutils.h>

// load shader module form .spv file content.
static VkShaderModule load_shader_module(VkDevice device, const char *name)
{
    char *buf;
    size_t size;
    VkResult U_ASSERT_ONLY err;

    buf = read_file_binary(name, &size);

    VkShaderModuleCreateInfo shader_module_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* codeSize */ size,
            /* pCode */ reinterpret_cast<uint32_t *>(buf),
    };

    VkShaderModule shader_module;
    err = vkCreateShaderModule(device, &shader_module_create_info, allocation_callbacks, &shader_module);
    assert(!err);

    free_file_binary(buf);

    return shader_module;
}

#endif /* _VULKAN_UTILS_H */
