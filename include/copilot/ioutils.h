/* ======================================================================== */
/* ioutils.h                                                                */
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
#ifndef _IOUTILS_H_
#define _IOUTILS_H_

#include <fstream>
#include <copilot/memalloc.h>

static char *read_file_binary(const char *path, size_t *size)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("error open file failed!");

    *size = file.tellg();
    file.seekg(0);

    /* malloc buffer */
    char *buf = (char *) imalloc(*size);
    file.read(buf, *size);
    file.close();

    return buf;
}

static void free_file_binary(char *buf)
{
    free(buf);
}

#endif /* _IOUTILS_H_ */
