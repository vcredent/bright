/* ======================================================================== */
/* obj.h                                                                    */
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
#ifndef _FORMAT_OBJ_H_
#define _FORMAT_OBJ_H_

#include <bright/math.h>
#include <bright/typedefs.h>
#include <vector>

class ObjLoader {
public:
    ~ObjLoader() { /* do nothing... */ }

    struct Vertex {
        Vec3 position;
        Vec2 texcoord;
        Vec3 normal;

        // eq
        bool operator==(const Vertex &other) const {
          return position == other.position && normal == other.normal && normal == other.normal &&
                 texcoord == other.texcoord;
        }
    };

    const std::vector<Vertex> &get_vertices() const { return vertices; }
    const std::vector<uint32_t> &get_indices() const { return indices; }

    // static
    static ObjLoader *load(const char *filepath);
    static void destroy(ObjLoader *loader);

private:
    U_MEMNEW_ONLY ObjLoader() { /* do nothing... */ }

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

#endif /* _FORMAT_OBJ_H_ */
