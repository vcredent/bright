/* ======================================================================== */
/* math.h                                                              */
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
#ifndef _BRIGHT_MATH_H_
#define _BRIGHT_MATH_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;
typedef glm::quat quat;

#define CONSOLE_WRITE_MATRIX_4x4(matrix) do {   \
    printf(                                     \
        "| %f   %f   %f   %f |\n"               \
        "| %f   %f   %f   %f |\n"               \
        "| %f   %f   %f   %f |\n"               \
        "| %f   %f   %f   %f |\n",              \
        matrix[0][0],                           \
        matrix[0][1],                           \
        matrix[0][2],                           \
        matrix[0][3],                           \
        matrix[1][0],                           \
        matrix[1][1],                           \
        matrix[1][2],                           \
        matrix[1][3],                           \
        matrix[2][0],                           \
        matrix[2][1],                           \
        matrix[2][2],                           \
        matrix[2][3],                           \
        matrix[3][0],                           \
        matrix[3][1],                           \
        matrix[3][2],                           \
        matrix[3][3]                            \
    );                                          \
} while(0)


#endif /* _BRIGHT_MATH_H_ */
