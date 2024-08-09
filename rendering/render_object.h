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
#include "physical3d/physical_3d.h"

class RenderObject : public NodeProperties {
public:
    RenderObject();
    ~RenderObject();

    struct Mesh {
        vec3 vertex;
        vec2 texcoord;
        vec3 normal;
    };

    void update();
    void initialize(RenderDevice *v_rd, Physical3D *v_physical);

    V_FORCEINLINE const char *get_name() { return name; }
    V_FORCEINLINE vec3 &get_object_position() { return position; }
    V_FORCEINLINE vec3 &get_object_rotation() { return rotation; }
    V_FORCEINLINE vec3 &get_object_scaling() { return scaling; }
    V_FORCEINLINE mat4 &get_model_matrix() { return transform; }
    V_FORCEINLINE Physical3DRigidBody *build_rigid_body_attributes() { return rb; }

    V_FORCEINLINE void set_name(const char *v_name) { name = v_name; }
    V_FORCEINLINE void set_object_position(vec3 v_position) { position = v_position; }
    V_FORCEINLINE void set_object_rotation(vec3 v_rotation) { rotation = v_rotation; }
    V_FORCEINLINE void set_object_scaling(vec3 v_scaling) { scaling = v_scaling; }

    void cmd_bind(VkCommandBuffer cmd_buffer);
    void cmd_draw(VkCommandBuffer cmd_buffer, RenderDevice::Pipeline *pipeline);

    static RenderObject *load_obj(const char *filename);

private:
    Physical3D *physical;
    Physical3DRigidBody *rb;

    std::vector<Mesh> meshes;
    std::vector<uint32_t> indices;
    mat4 transform = mat4(1.0f);
    RenderDevice *rd;

    vec3 position = vec3(0.0f);
    vec3 rotation = vec3(0.0f);
    vec3 scaling = vec3(1.0f);

    const char *name;
    RenderDevice::Buffer *vertex_buffer = VK_NULL_HANDLE;
    RenderDevice::Buffer *index_buffer = VK_NULL_HANDLE;
};

#endif /* _GRAPHICS_OBJECT_H_ */
