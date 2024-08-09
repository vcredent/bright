/* ======================================================================== */
/* physical_3d_space.h                                                      */
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
#ifndef _PHYSICAL_3D_SPACE_H_
#define _PHYSICAL_3D_SPACE_H_

#include "physical_3d_world.h"

class Physical3DSpace {
public:
    Physical3DSpace();
   ~Physical3DSpace();

    dSpaceID get_ode_space_id() { return space; }
    void update_collide(Physical3DWorld *v_world);

private:
    dSpaceID space;
};

#endif /* _PHYSICAL_3D_SPACE_H_ */