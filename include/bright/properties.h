/* ======================================================================== */
/* properties.h                                                              */
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
#ifndef _CLASS_PROPERTIES_H_
#define _CLASS_PROPERTIES_H_

#include <string>
#include <vector>
#include <map>

enum NodePropertyType {
    FLOAT,
    FLOAT2,
    FLOAT3,
    COLOR,
    SLIDER,
};

class NodeProperties {
public:
    struct Property {
        const char* name;
        NodePropertyType type;
        void *ptr;
        float min;
        float max;
        float speed;
    };

    struct NodeGroup {
        std::string name;
        std::vector<Property> properties;
        void add_property(const char* name, NodePropertyType type, void* ptr, float min = 0.0f, float max = 10.0f, float speed = 0.01f)
          {
            properties.push_back({ name, type, ptr, min, max, speed });
          }
    };

    void set_node_name(const char *v_name) { name = v_name; }
    const char *get_node_name() { return name; }
    void set_node_icon(const char *v_icon) { icon = v_icon; }
    const char *get_node_icon() { return icon; }
    const std::map<std::string, NodeGroup>& get_node_groups() { return groups; }

    NodeGroup* get_node_group(std::string name) 
      { 
        auto it = groups.find(name);
        if (it == groups.end()) {
            NodeGroup group = {};
            group.name = name;
            groups.insert({ name, group });
            it = groups.find(name);
        }
        return &it->second;
      }

private:
    const char *name;
    const char *icon = NULL;
    std::map<std::string, NodeGroup> groups;
};

#endif /* _CLASS_PROPERTIES_H_ */
