/* ************************************************************************ */
/* typedefs.h                                                               */
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
#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

#define stackalloc() {/* 0 */}

#if defined(__MINGW32__)
#  define V_FORCEINLINE __attribute__((__always_inline__)) inline
#elif defined(_MSC_VER)
#  define V_FORCEINLINE __forceinline
#else
#  define V_FORCEINLINE
#endif

#if defined(__MINGW32__)
#  define U_MAYBE_UNUSED __attribute__((unused))
#else
#  define U_MAYBE_UNUSED
#endif

#define U_ASSERT_ONLY U_MAYBE_UNUSED
#define U_MEMNEW_ONLY U_MAYBE_UNUSED
#define ARRAY_SIZE(a) ( sizeof(a) / sizeof(a[0]) )

#if defined(__MINGW32__)
#  define RESOURCE(path) "../Engine/Resources" path
#elif defined(_MSC_VER)
#  define RESOURCE(path) "../../../Engine/Resources" path
#endif

// std::string to const char *
#define getpchar(str) ( str.c_str() )

#endif /* _TYPEDEFS_H_ */