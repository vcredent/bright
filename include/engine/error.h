/* ************************************************************************ */
/* Error.h                                                                  */
/* ************************************************************************ */
/*                        This file is part of:                             */
/*                           PORTABLE ENGINE                                */
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
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, e1ither express or implied */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ************************************************************************ */
#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdio.h>
#include <stdexcept>
#include <assert.h>

#define EXIT_FAIL(...) do {            \
    fprintf(stderr, __VA_ARGS__);      \
    exit(1);                           \
} while(0)

#define EXIT_FAIL_V(retval, ...) do {  \
    if (!(retval)) {                   \
        EXIT_FAIL(__VA_ARGS__);        \
    }                                  \
} while(0)

enum Error {
    OK = 0,
    FAIL,
};

#endif /* _ERROR_H_ */