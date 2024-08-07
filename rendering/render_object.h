/* ======================================================================== */
/* render_object.h                                                          */
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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, e1ither express or implied */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#ifndef _GRAPHICS_OBJECT_H_
#define _GRAPHICS_OBJECT_H_

#include "drivers/render_device.h"
#include <bright/math.h>
#include <bright/properties.h>

class RenderObject : public NodeProperties {
public:
    RenderObject();
    ~RenderObject();

    struct Mesh {
        Vec3 vertex;
        Vec2 texcoord;
        Vec3 normal;
    };

    void update();
    void initialize(RenderDevice *v_rd);

    V_FORCEINLINE const char *get_name() { return name; }
    V_FORCEINLINE Vec3 &get_object_position() { return position; }
    V_FORCEINLINE Vec3 &get_object_rotation() { return rotation; }
    V_FORCEINLINE Vec3 &get_object_scaling() { return scaling; }
    V_FORCEINLINE Mat4 &get_model_matrix() { return transform; }

    V_FORCEINLINE void set_name(const char *v_name) { name = v_name; }
    V_FORCEINLINE void set_object_position(Vec3 &v_position) { position = v_position; }
    V_FORCEINLINE void set_object_rotation(Vec3 &v_rotation) { rotation = v_rotation; }
    V_FORCEINLINE void set_object_scaling(Vec3 &v_scaling) { scaling = v_scaling; }

    void cmd_bind(VkCommandBuffer cmd_buffer);
    void cmd_draw(VkCommandBuffer cmd_buffer, RenderDevice::Pipeline *pipeline);

    static RenderObject *load_obj(const char *filename);

private:
    std::vector<Mesh> meshes;
    std::vector<uint32_t> indices;
    Mat4 transform = Mat4(1.0f);
    RenderDevice *rd;

    Vec3 position = Vec3(0.0f);
    Vec3 rotation = Vec3(0.0f);
    Vec3 scaling = Vec3(1.0f);

    const char *name;
    RenderDevice::Buffer *vertex_buffer = VK_NULL_HANDLE;
    RenderDevice::Buffer *index_buffer = VK_NULL_HANDLE;
};

#endif /* _GRAPHICS_OBJECT_H_ */
