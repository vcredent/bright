/* ======================================================================== */
/* physical_3d_rigid_body.h                                                 */
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
#ifndef _PHYSICAL_3D_RIGID_BODY_H_
#define _PHYSICAL_3D_RIGID_BODY_H_

#include <bright/math.h>
#include "physical_3d_world.h"
#include "physical_3d_space.h"

class Physical3DRigidBody {
public:
    Physical3DRigidBody(Physical3DWorld *v_world, Physical3DSpace *v_space);
   ~Physical3DRigidBody();

    vec3 get_simulate_position();
    vec3 get_simulate_rotate();

    void set_body_density(float v_density) { density = v_density; }
    void set_body_volume(float width, float height, float depth) { volume = vec3(width, height, depth); }
    void set_body_position(vec3 v_position) { set_body_position(v_position.x, v_position.y, v_position.z); }
    void set_body_position(float x, float y, float z) { position = vec3(x, y, z); }
    void set_body_velocity(float x, float y, float z) { velocity = vec3(x, y, z); }
    void set_body_angular_velocity(float x, float y, float z) { angular_velocity = vec3(x, y, z); }

    void create_rigid_body();

private:
    Physical3DWorld *world;
    Physical3DSpace *space;
    dMass mass;
    dBodyID body;
    dGeomID geom;

    float density = 0.0f;
    vec3 position = vec3(0.0f);
    vec3 rotate = vec3(0.0f);
    vec3 volume = vec3(1.0f);
    vec3 velocity = vec3(0.0f);
    vec3 angular_velocity = vec3(0.0f);
};

#endif /* _PHYSICAL_3D_RIGID_BODY_H_ */
