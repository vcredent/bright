/* ************************************************************************ */
/* memalloc.h                                                               */
/* ************************************************************************ */
/*                        This file is part of:                             */
/*                            BRIGHT ENGINE                                 */
/* ************************************************************************ */
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
/* ************************************************************************ */
#ifndef _MEMALLOC_H_
#define _MEMALLOC_H_

#include <stdlib.h>
#include <string.h>
#include <memory>

/* use follow the memory allocate */
#define _post_initialize(ptr) (ptr)

// allocate memory and initialize members to zero,
// the default malloc does not initialize members
inline static void *imalloc(size_t size)
{
    void *ptr;

    ptr = malloc(size);
    memset(ptr, 0, size);

    return _post_initialize(ptr);
}

#define memnew(obj, ...) _post_initialize(new obj(__VA_ARGS__))
#define memdel(ptr) delete (ptr)

#endif /* _MEMALLOC_H_ */