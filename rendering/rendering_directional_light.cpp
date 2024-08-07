/* ======================================================================== */
/* rendering_directional_light.cpp                                          */
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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied  */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#include "rendering_directional_light.h"

RenderingDirectionalLight::RenderingDirectionalLight(RenderDevice *v_rd)
{
    set_node_name("定向光源");
    set_node_icon("sun");

    NodeGroup* group;

    group = get_node_group("变换");
    group->add_property("方向", NodePropertyType::FLOAT3, glm::value_ptr(direction));

    group = get_node_group("属性");
    group->add_property("颜色", NodePropertyType::FLOAT3, glm::value_ptr(color));
    group->add_property("高光", NodePropertyType::FLOAT3, glm::value_ptr(specular_color));
    group->add_property("强度", NodePropertyType::FLOAT, &intensity);
    group->add_property("指数", NodePropertyType::FLOAT, &specular_exponent);
    group->add_property("环境", NodePropertyType::FLOAT, &ambient);
}

RenderingDirectionalLight::~RenderingDirectionalLight()
{
    /* do nothing... */
}

void RenderingDirectionalLight::copy_data(SceneRenderData::DirectionalLight* v_light)
{
    v_light->direction = direction;
    v_light->color = color;
    v_light->specular_color = specular_color;
    v_light->intensity = intensity;
    v_light->specular_exponent = specular_exponent;
    v_light->ambient = ambient;
}