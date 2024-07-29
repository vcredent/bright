/* ======================================================================== */
/* obj.h                                                                    */
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
#ifndef _FORMAT_OBJ_H_
#define _FORMAT_OBJ_H_

#include <copilot/math.h>
#include <copilot/typedefs.h>
#include <vector>

class ObjLoader {
public:
    ~ObjLoader() { /* do nothing... */ }

    struct Vertex {
        Vec3 v;
        Vec2 vt;
        Vec3 vn;
    };

    struct Face {
        std::vector<int> vertices;
    };

    static ObjLoader *load(const char *filepath);
    static void destroy(ObjLoader *loader);

    const std::vector<Vertex> &get_obj_vertices() const { return vertices; }
    const std::vector<Face> &get_obj_faces() const { return faces; }

private:
    U_MEMNEW_ONLY ObjLoader() { /* do nothing... */ }

    std::vector<Vertex> vertices;
    std::vector<Face> faces;
};

#endif /* _FORMAT_OBJ_H_ */
