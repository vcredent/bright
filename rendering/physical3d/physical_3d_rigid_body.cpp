/* ======================================================================== */
/* physical_3d_rigid_body.cpp                                               */
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
#include "physical_3d_rigid_body.h"

Physical3DRigidBody::Physical3DRigidBody(Physical3DWorld *v_world, Physical3DSpace *v_space)
    : world(v_world), space(v_space)
{
    /* do nothing... */
}

Physical3DRigidBody::~Physical3DRigidBody()
{
    dBodyDestroy(body);
    dGeomDestroy(geom);
}

vec3 Physical3DRigidBody::get_simulate_position()
{
    const dReal *v_real;
    v_real = dBodyGetPosition(body);
    return vec3(v_real[0], v_real[1], v_real[2]);
}

vec3 Physical3DRigidBody::get_simulate_rotate()
{
    const dReal *v_real;
    v_real = dBodyGetRotation(body);
    return vec3(
        v_real[0],
        v_real[1],
        v_real[2]
    );
}

void Physical3DRigidBody::create_rigid_body()
{
    body = dBodyCreate(world->get_ode_world_id());
    dMassSetBoxTotal(&mass, density, volume.x, volume.y, volume.z);
    dBodySetMass(body, &mass);

    dBodySetPosition(body, position.x, position.y, position.z);
    dBodySetLinearVel(body, velocity.x, velocity.y, velocity.z);
    dBodySetAngularVel(body, angular_velocity.x, angular_velocity.y, angular_velocity.z);

    geom = dCreateBox(space->get_ode_space_id(), volume.x, volume.y, volume.z);
    dGeomSetBody(geom, body);
}
