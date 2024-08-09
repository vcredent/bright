/* ======================================================================== */
/* shader_compile.h                                                         */
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
#ifndef _SHADER_COMPILE_H_
#define _SHADER_COMPILE_H_

#include <stdlib.h>
#include <bright/typedefs.h>
#include <direct.h>

V_FORCEINLINE static void compile_shader()
{
    char cwd[512];

    system("chcp 65001");
    getcwd(cwd, sizeof(cwd));
    _chdir(_CURDIR("shader"));
    system("compile.bat");
    _chdir(cwd);
}

#endif /* _SHADER_COMPILE_H_ */
