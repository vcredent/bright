/* ======================================================================== */
/* physical_3d_world.h                                                      */
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
#ifndef _PHYSICAL_3D_WORLD_H_
#define _PHYSICAL_3D_WORLD_H_

#include <ode/ode.h>

class Physical3DWorld {
public:
    Physical3DWorld();
   ~Physical3DWorld();

    float get_gravity() { return gravity; }
    dWorldID get_ode_world_id() { return world; }
    dJointGroupID get_ode_join_group_id() { return concat; }

    void set_gravity(float v_gravity);

    void update_step();

private:
    dWorldID world;
    dJointGroupID concat;

    float gravity = 9.81;
    float time_step = 0.02f;
};

#endif /* _PHYSICAL_3D_WORLD_H_ */