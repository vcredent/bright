/* ======================================================================== */
/* render_object.h                                                          */
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
#ifndef _GRAPHICS_OBJECT_H_
#define _GRAPHICS_OBJECT_H_

#include "drivers/render_device.h"
#include <copilot/math.h>

class RenderObject {
public:
    RenderObject();
    ~RenderObject();

    struct Mesh {
        Vector3 vertex;
        Vector2 texcoord;
        Vector3 normal;
    };

    void update();
    void initialize(RenderDevice *v_rd);

    V_FORCEINLINE Vector3 &get_object_position() { return position; }
    V_FORCEINLINE Vector3 &get_object_rotation() { return rotation; }
    V_FORCEINLINE Vector3 &get_object_scaling() { return scaling; }
    V_FORCEINLINE Matrix4 &get_model_matrix() { return transform; }

    V_FORCEINLINE void set_object_position(Vector3 &v_position) { position = v_position; }
    V_FORCEINLINE void set_object_rotation(Vector3 &v_rotation) { rotation = v_rotation; }
    V_FORCEINLINE void set_object_scaling(Vector3 &v_scaling) { scaling = v_scaling; }

    void cmd_bind_vertex_buffer(VkCommandBuffer cmd_buffer);
    void cmd_bind_index_buffer(VkCommandBuffer cmd_buffer);
    void cmd_draw(VkCommandBuffer cmd_buffer);

    static RenderObject *load_assets_obj(const char *filename);

private:
    std::vector<Mesh> meshes;
    std::vector<uint32_t> indices;
    Matrix4 transform;
    RenderDevice *rd;

    Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 scaling = Vector3(0.0f, 0.0f, 0.0f);

    RenderDevice::Buffer *vertex_buffer = VK_NULL_HANDLE;
    RenderDevice::Buffer *index_buffer = VK_NULL_HANDLE;
};

#endif /* _GRAPHICS_OBJECT_H_ */
