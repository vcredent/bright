/* ======================================================================== */
/* rendering_directional_light.h                                            */
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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied  */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#ifndef _RENDERING_DIRECTIONAL_LIGHT_H_
#define _RENDERING_DIRECTIONAL_LIGHT_H_

#include "drivers/render_device.h"
#include "scene_render_data.h"
#include <bright/properties.h>

class RenderingDirectionalLight : public NodeProperties {
public:
    RenderingDirectionalLight(RenderDevice* v_rd);
   ~RenderingDirectionalLight();

    void copy_data(SceneRenderData::DirectionalLight *v_light);

    V_FORCEINLINE vec3 &get_direction() { return direction; }
    V_FORCEINLINE vec3 &get_color() { return color; }
    V_FORCEINLINE vec3 &get_specular_color() { return specular_color; }
    V_FORCEINLINE float get_intensity() { return intensity; }
    V_FORCEINLINE float get_specular_exponent() { return specular_exponent; }
    V_FORCEINLINE float get_ambient() { return ambient; }

private:
    vec3 direction = vec3(-1.0f);
    vec3 color = vec3(1.0f);
    vec3 specular_color = vec3(1.0f);
    float intensity = 1.0f;
    float specular_exponent = 32.0f;
    float ambient = 0.3f;
};

#endif /* _RENDERING_DIRECTIONAL_LIGHT_H_ */
