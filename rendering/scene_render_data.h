/* ======================================================================== */
/* scene_render_data.h                                                      */
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
#ifndef _SCENE_RENDER_DATA_H_
#define _SCENE_RENDER_DATA_H_

#include "drivers/render_device.h"
#include <copilot/math.h>

class SceneRenderData {
public:
    U_MEMNEW_ONLY SceneRenderData(RenderDevice *v_rd);
   ~SceneRenderData();

    struct Perspective {
        Vec4 camera_pos;
        Mat4 projection;
        Mat4 view;
    };

    struct DirectionalLight {
        Vec3 direction;
        float intensity;
        Vec3 color;
        float specular_exponent;
        Vec3 specular_color;
        float ambient;
    };

    // api
    void set_render_data(uint32_t v_width,
                         uint32_t v_height,
                         Perspective *v_perspective,
                         DirectionalLight *v_light);

    V_FORCEINLINE uint32_t get_scene_width() { return width; }
    V_FORCEINLINE uint32_t get_scene_height() { return height; }
    V_FORCEINLINE RenderDevice::Buffer* get_perspective_buffer() { return perspective_buffer; }
    V_FORCEINLINE RenderDevice::Buffer* get_directional_light_buffer() { return directional_light_buffer; }

    V_FORCEINLINE void set_descriptor_buffers(VkDescriptorSet descriptor)
      {
        rd->update_descriptor_set_buffer(perspective_buffer, 0, descriptor);
        rd->update_descriptor_set_buffer(directional_light_buffer, 1, descriptor);
      }

    /* get descriptor bind zero. */
    static VkDescriptorSetLayoutBinding GetPerspectiveDescriptorBindZero()
      {
        VkDescriptorSetLayoutBinding bind = {
          /* binding= */ 0,
          /* descriptorType= */ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          /* descriptorCount= */ 1,
          /* stageFlags= */ VK_SHADER_STAGE_VERTEX_BIT,
          /* pImmutableSamplers= */ VK_NULL_HANDLE
        };
        return bind;
      }

    static VkDescriptorSetLayoutBinding GetLightDescriptorBindOne()
      {
        VkDescriptorSetLayoutBinding bind = {
          /* binding= */ 1,
          /* descriptorType= */ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          /* descriptorCount= */ 1,
          /* stageFlags= */ VK_SHADER_STAGE_FRAGMENT_BIT,
          /* pImmutableSamplers= */ VK_NULL_HANDLE
        };
        return bind;
      }

private:
    RenderDevice *rd;
    uint32_t width;
    uint32_t height;
    RenderDevice::Buffer * perspective_buffer;
    RenderDevice::Buffer * directional_light_buffer;
};

#endif /* _SCENE_RENDER_DATA_H_ */
