/* ======================================================================== */
/* physical_3d_world.cpp                                                    */
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
#include "physical_3d_world.h"

Physical3DWorld::Physical3DWorld()
{
    world = dWorldCreate();
    concat = dJointGroupCreate(0);
    dWorldSetGravity(world, 0.0f, gravity, 0.0f);
}

Physical3DWorld::~Physical3DWorld()
{
    dWorldDestroy(world);
}

void Physical3DWorld::set_gravity(float v_gravity)
{
    gravity = v_gravity;
}

void Physical3DWorld::update_step()
{
    dWorldStep(world, time_step);
    dJointGroupEmpty(concat);
}
