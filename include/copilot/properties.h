/* ======================================================================== */
/* properties.h                                                              */
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
#ifndef _CLASS_PROPERTIES_H_
#define _CLASS_PROPERTIES_H_

#include <unordered_map>

enum PropertyType {
    FLOAT,
    FLOAT2,
    FLOAT3,
};

class ClassProperties {
public:
    struct Property {
        void *ptr;
        PropertyType type;
    };

    void set_node_name(const char *v_name) { name = v_name; }
    const char *get_node_name() { return name; }
    void set_node_icon(const char *v_icon) { icon = v_icon; }
    const char *get_node_icon() { return icon; }
    void add_node_property(const char *name, PropertyType type, void *ptr) { properties.insert({ name, { ptr, type } }); }
    const std::unordered_map<const char *, Property> &get_node_properties() { return properties; }

private:
    const char *name;
    const char *icon = NULL;
    std::unordered_map<const char *, Property> properties;
};

#endif /* _CLASS_PROPERTIES_H_ */
