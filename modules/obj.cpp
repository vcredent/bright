/* ======================================================================== */
/* obj.cpp                                                                  */
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
#include "obj.h"
#include <bright/memalloc.h>
#include <bright/error.h>
#include <tinyobjloader/tiny_obj_loader.h>
#include <unordered_map>

namespace std {
  template <>
  struct hash<ObjLoader::Vertex> {
    size_t operator()(ObjLoader::Vertex const &vertex) const {
      std::size_t h1 = std::hash<float>{}(vertex.position.x);
      std::size_t h2 = std::hash<float>{}(vertex.position.y);
      std::size_t h3 = std::hash<float>{}(vertex.position.z);
      std::size_t h4 = std::hash<float>{}(vertex.texcoord.x);
      std::size_t h5 = std::hash<float>{}(vertex.texcoord.y);
      std::size_t h6 = std::hash<float>{}(vertex.normal.x);
      std::size_t h7 = std::hash<float>{}(vertex.normal.y);
      std::size_t h8 = std::hash<float>{}(vertex.normal.z);
      // Combine hash values
      return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5) ^ (h7 << 6) ^ (h8 << 7);
    }
  };
}

ObjLoader *ObjLoader::load(const char *filepath)
{
    ObjLoader *loader = memnew(ObjLoader);

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filepath))
        EXIT_FAIL("-engine error: parse .obj model failed!");

    const tinyobj::attrib_t& attrib = reader.GetAttrib();
    const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
    std::unordered_map<Vertex, uint32_t> unique_vertices = {};

    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            Vertex vertex = {};
            if (index.vertex_index >= 0) {
                vertex.position = {
                  attrib.vertices[3 * index.vertex_index + 0],
                  attrib.vertices[3 * index.vertex_index + 1],
                  attrib.vertices[3 * index.vertex_index + 2],
                };
            }

            if (index.texcoord_index >= 0) {
                vertex.texcoord = {
                  attrib.texcoords[2 * index.texcoord_index + 0],
                  attrib.texcoords[2 * index.texcoord_index + 1],
                };
            }

            if (index.normal_index >= 0) {
                vertex.normal = {
                  attrib.normals[3 * index.normal_index + 0],
                  attrib.normals[3 * index.normal_index + 1],
                  attrib.normals[3 * index.normal_index + 1],
                };
            }

            if (unique_vertices.count(vertex) == 0) {
                unique_vertices[vertex] = static_cast<uint32_t>(loader->vertices.size());
                loader->vertices.push_back(vertex);
            }

            loader->indices.push_back(unique_vertices[vertex]);
        }
    }

    return loader;
}

void ObjLoader::destroy(ObjLoader *loader)
{
    memdel(loader);
}