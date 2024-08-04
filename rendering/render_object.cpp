/* ======================================================================== */
/* render_object.cpp                                                        */
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
#include "render_object.h"
#include "modules/obj.h"
#include <tinyobjloader/tiny_obj_loader.h>

RenderObject::RenderObject()
{
    /* do nothing... */
}

RenderObject::~RenderObject()
{
    rd->destroy_buffer(vertex_buffer);
    rd->destroy_buffer(index_buffer);
}

void RenderObject::update()
{
    Mat4 rotate(1.0f);
    Mat4 scale(1.0f);
    Mat4 translate(1.0f);

    static float sensitivity = 8.0f;
    rotate = glm::rotate(rotate, glm::radians(rotation.x * sensitivity), Vec3(1.0f, 0.0f, 0.0f));
    rotate = glm::rotate(rotate, glm::radians(rotation.y * sensitivity), Vec3(0.0f, 1.0f, 0.0f));
    rotate = glm::rotate(rotate, glm::radians(rotation.z * sensitivity), Vec3(0.0f, 0.0f, 1.0f));

    scale = glm::scale(scale, scaling);

    translate = glm::translate(translate, position);

    transform = translate * scale * rotate;
}

void RenderObject::initialize(RenderDevice *v_rd)
{
    rd = v_rd;

    size_t vertex_buffer_size = std::size(meshes) * sizeof(Mesh);
    size_t index_buffer_size = std::size(indices) * sizeof(uint32_t);

    vertex_buffer = rd->create_buffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertex_buffer_size);
    index_buffer = rd->create_buffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, index_buffer_size);

    rd->write_buffer(vertex_buffer, 0, vertex_buffer_size, std::data(meshes));
    rd->write_buffer(index_buffer, 0, index_buffer_size, std::data(indices));
}

void RenderObject::cmd_draw(VkCommandBuffer cmd_buffer, RenderDevice::Pipeline *pipeline)
{
    rd->cmd_bind_vertex_buffer(cmd_buffer, vertex_buffer);
    rd->cmd_bind_index_buffer(cmd_buffer, VK_INDEX_TYPE_UINT32, index_buffer);
    rd->cmd_push_const(cmd_buffer, pipeline, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Mat4), &transform);
    rd->cmd_draw_indexed(cmd_buffer, std::size(indices));
}

RenderObject *RenderObject::load_obj(const char *filename)
{
    RenderObject *object = memnew(RenderObject);

    ObjLoader *loader = ObjLoader::load(filename);

    std::vector<ObjLoader::Vertex> vertices = loader->get_vertices();
    std::vector<uint32_t> indices = loader->get_indices();

    for (const auto &vertex: vertices) {
        object->meshes.push_back({
          vertex.position,
          vertex.texcoord,
          vertex.normal,
        });
    }

    for (const auto &index: indices) {
        object->indices.push_back(index);
    }

    ObjLoader::destroy(loader);

    return object;
}