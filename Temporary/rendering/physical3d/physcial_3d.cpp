/* ======================================================================== */
/* physical_3d.cpp                                                          */
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
#include "physical_3d.h"

Physical3D::Physical3D()
{
    dInitODE();

    world = memnew(Physical3DWorld);
    space = memnew(Physical3DSpace);
}

Physical3D::~Physical3D()
{
    memdel(world);
    memdel(space);
    dCloseODE();
}

Physical3DRigidBody *Physical3D::create_rigid_body()
{
    return memnew(Physical3DRigidBody, world, space);
}

void Physical3D::destroy_rigid_body(Physical3DRigidBody *rb)
{
    memdel(rb);
}

void Physical3D::update_physical_world()
{
    space->update_collide(world);
    world->update_step();
}