/* ======================================================================== */
/* obj.cpp                                                                  */
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
#include "obj.h"

#include <copilot/memalloc.h>
#include <copilot/ioutils.h>
#include <copilot/error.h>
#include <sstream>

ObjLoader *ObjLoader::load(const char *filepath)
{
    ObjLoader *loader = memnew(ObjLoader);

    std::ifstream file(filepath);
    EXIT_FAIL_COND_V(file.is_open(), ".obj loader open fil failed!");

    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<Vec2> texcoords;
    std::vector<Face> faces;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            Vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
            goto CONTINUE_TAG;
        }

        if (prefix == "vn") {
            Vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
            goto CONTINUE_TAG;
        }

        if (prefix == "vt") {
            Vec2 texcoord;
            ss >> texcoord.x >> texcoord.y;
            texcoords.push_back(texcoord);
            goto CONTINUE_TAG;
        }

        if (prefix == "f") {
            Face face;

            std::string vertex;
            while (ss >> vertex) {
                size_t pos1 = vertex.find("/");
                int index = std::stoi(vertex.substr(0, pos1)) - 1;
                face.vertices.push_back(index);
            }
            faces.push_back(face);

            goto CONTINUE_TAG;
        }
CONTINUE_TAG:
    }

    for (int i = 0; i < std::size(vertices); i++) {
        Vertex vertex;
        vertex.v = vertices[i];
        vertex.vt = texcoords[i];
        vertex.vn = normals[i];
        loader->vertices.push_back(vertex);
    }

    loader->faces = faces;
    file.close();

    return loader;
}