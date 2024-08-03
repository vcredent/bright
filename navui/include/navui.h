/* ======================================================================== */
/* navui.h                                                                  */
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
#ifndef _PILOTUI_H_
#define _PILOTUI_H_

#include <imgui.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

typedef void *NavTextureId;

namespace NavUI {

    struct InitializeInfo {
        GLFWwindow*                     window;
        VkInstance                      Instance;
        VkPhysicalDevice                PhysicalDevice;
        VkDevice                        Device;
        uint32_t                        QueueFamily;
        VkQueue                         Queue;
        VkDescriptorPool                DescriptorPool;
        VkRenderPass                    RenderPass;
        uint32_t                        MinImageCount;
        uint32_t                        ImageCount;
        VkSampleCountFlagBits           MSAASamples;
    };

    // create and destroy
    void Initialize(InitializeInfo *p_initialize_info);
    void Destroy();

    // api
    void BeginNewFrame(VkCommandBuffer cmd_buffer);
    void EndNewFrame(VkCommandBuffer cmd_buffer);
    void Begin(const char *title);
    void End();

    // vulkan
    NavTextureId AddTexture(VkSampler v_sampler, VkImageView v_image, VkImageLayout v_layout);
    void RemoveTexture(NavTextureId v_texture);

}

#endif /* _PILOTUI_H_ */
