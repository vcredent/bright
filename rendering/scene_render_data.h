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

    struct UniformBuffer {
        Vec4 camera_pos;
        Mat4 projection;
        Mat4 view;
    };

    // api
    void set_render_data(uint32_t v_width,
                         uint32_t v_height,
                         const Vec3 &camera_position,
                         const Mat4 &projection,
                         const Mat4 &view);

    V_FORCEINLINE uint32_t get_scene_width() { return width; }
    V_FORCEINLINE uint32_t get_scene_height() { return height; }
    V_FORCEINLINE RenderDevice::Buffer *get_uniform_buffer() { return buffer; }

    // static
    V_FORCEINLINE static size_t GetUniformSize() { return sizeof(UniformBuffer); }

    /* get descriptor bind zero. */
    static VkDescriptorSetLayoutBinding GetDescriptorBindZero()
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

private:
    RenderDevice *rd;
    uint32_t width;
    uint32_t height;
    UniformBuffer uniform;
    RenderDevice::Buffer *buffer;
};

#endif /* _SCENE_RENDER_DATA_H_ */
