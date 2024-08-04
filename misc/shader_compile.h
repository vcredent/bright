/* ======================================================================== */
/* shader_compile.h                                                         */
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
#ifndef _SHADER_COMPILE_H_
#define _SHADER_COMPILE_H_

#include <stdlib.h>
#include <copilot/typedefs.h>
#include <direct.h>

V_FORCEINLINE
inline static void compile_shader()
{
    system("chcp 65001");
    _chdir("../shader");
    system("compile.bat");
}

#endif /* _SHADER_COMPILE_H_ */
