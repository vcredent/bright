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
#include "model/obj.h"
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
    Matrix4 rotate(1.0f);
    Matrix4 translate(1.0f);
    Matrix4 scale(1.0f);

    translate = glm::translate(translate, position);

    transform = translate;
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

void RenderObject::cmd_bind_vertex_buffer(VkCommandBuffer cmd_buffer)
{
    rd->cmd_bind_vertex_buffer(cmd_buffer, vertex_buffer);
}

void RenderObject::cmd_bind_index_buffer(VkCommandBuffer cmd_buffer)
{
    rd->cmd_bind_index_buffer(cmd_buffer, VK_INDEX_TYPE_UINT32, index_buffer);
}

void RenderObject::cmd_draw(VkCommandBuffer cmd_buffer)
{
    rd->cmd_draw_indexed(cmd_buffer, std::size(indices));
}

RenderObject *RenderObject::load_assets_obj(const char *filename)
{
    RenderObject *object = memnew(RenderObject);
    ObjLoader *loader = ObjLoader::load(filename);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    tinyobj::LoadObj(&attrib, &shapes, &materials, NULL, NULL, filename);

    for (const auto& shape : shapes) {
        for (size_t i = 0; i < shape.mesh.indices.size(); ++i) {
            const auto& index = shape.mesh.indices[i];

            // 获取顶点、纹理坐标和法线数据
            Vector3 vertex = { attrib.vertices[3 * index.vertex_index + 0],
                               attrib.vertices[3 * index.vertex_index + 1],
                               attrib.vertices[3 * index.vertex_index + 2] };

            Vector2 texcoord = { index.texcoord_index >= 0 ?
                                 attrib.texcoords[2 * index.texcoord_index + 0] : 0.0f,
                                 index.texcoord_index >= 0 ?
                                 attrib.texcoords[2 * index.texcoord_index + 1] : 0.0f };

            Vector3 normal = { index.normal_index >= 0 ?
                               attrib.normals[3 * index.normal_index + 0] : 0.0f,
                               index.normal_index >= 0 ?
                               attrib.normals[3 * index.normal_index + 1] : 0.0f,
                               index.normal_index >= 0 ?
                               attrib.normals[3 * index.normal_index + 2] : 0.0f };

            // 将数据存入 mesh 结构体中
            Mesh mesh = {
                /* vertex= */ vertex,
                /* texcoord= */ texcoord,
                /* normal= */ normal
            };
            object->meshes.push_back(mesh);
            object->indices.push_back(static_cast<uint32_t>(i));
        }
    }

    memdel(loader);
    return object;
}