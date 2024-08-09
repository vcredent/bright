/* ======================================================================== */
/* inpdefs.h                                                                */
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
#ifndef _BRIGHT_INPDEFS_H_
#define _BRIGHT_INPDEFS_H_

#define EVENT_INPUT_RELEASE                0
#define EVENT_INPUT_PRESS                  1

/* even input of mouse button id, this mapping by glfw
 * defined macros value. */
#define EVENT_INPUT_MOUSE_BUTTON_1         0
#define EVENT_INPUT_MOUSE_BUTTON_2         1
#define EVENT_INPUT_MOUSE_BUTTON_3         2
#define EVENT_INPUT_MOUSE_BUTTON_4         3
#define EVENT_INPUT_MOUSE_BUTTON_5         4
#define EVENT_INPUT_MOUSE_BUTTON_6         5
#define EVENT_INPUT_MOUSE_BUTTON_7         6
#define EVENT_INPUT_MOUSE_BUTTON_8         7
#define EVENT_INPUT_MOUSE_BUTTON_LAST      EVENT_INPUT_MOUSE_BUTTON_8
#define EVENT_INPUT_MOUSE_BUTTON_LEFT      EVENT_INPUT_MOUSE_BUTTON_1
#define EVENT_INPUT_MOUSE_BUTTON_RIGHT     EVENT_INPUT_MOUSE_BUTTON_2
#define EVENT_INPUT_MOUSE_BUTTON_MIDDLE    EVENT_INPUT_MOUSE_BUTTON_3

/* The unknown key */
#define EVENT_INPUT_KEY_UNKNOWN            -1

/* Printable keys */
#define EVENT_INPUT_KEY_SPACE              32
#define EVENT_INPUT_KEY_APOSTROPHE         39  /* ' */
#define EVENT_INPUT_KEY_COMMA              44  /* , */
#define EVENT_INPUT_KEY_MINUS              45  /* - */
#define EVENT_INPUT_KEY_PERIOD             46  /* . */
#define EVENT_INPUT_KEY_SLASH              47  /* / */
#define EVENT_INPUT_KEY_0                  48
#define EVENT_INPUT_KEY_1                  49
#define EVENT_INPUT_KEY_2                  50
#define EVENT_INPUT_KEY_3                  51
#define EVENT_INPUT_KEY_4                  52
#define EVENT_INPUT_KEY_5                  53
#define EVENT_INPUT_KEY_6                  54
#define EVENT_INPUT_KEY_7                  55
#define EVENT_INPUT_KEY_8                  56
#define EVENT_INPUT_KEY_9                  57
#define EVENT_INPUT_KEY_SEMICOLON          59  /* ; */
#define EVENT_INPUT_KEY_EQUAL              61  /* = */
#define EVENT_INPUT_KEY_A                  65
#define EVENT_INPUT_KEY_B                  66
#define EVENT_INPUT_KEY_C                  67
#define EVENT_INPUT_KEY_D                  68
#define EVENT_INPUT_KEY_E                  69
#define EVENT_INPUT_KEY_F                  70
#define EVENT_INPUT_KEY_G                  71
#define EVENT_INPUT_KEY_H                  72
#define EVENT_INPUT_KEY_I                  73
#define EVENT_INPUT_KEY_J                  74
#define EVENT_INPUT_KEY_K                  75
#define EVENT_INPUT_KEY_L                  76
#define EVENT_INPUT_KEY_M                  77
#define EVENT_INPUT_KEY_N                  78
#define EVENT_INPUT_KEY_O                  79
#define EVENT_INPUT_KEY_P                  80
#define EVENT_INPUT_KEY_Q                  81
#define EVENT_INPUT_KEY_R                  82
#define EVENT_INPUT_KEY_S                  83
#define EVENT_INPUT_KEY_T                  84
#define EVENT_INPUT_KEY_U                  85
#define EVENT_INPUT_KEY_V                  86
#define EVENT_INPUT_KEY_W                  87
#define EVENT_INPUT_KEY_X                  88
#define EVENT_INPUT_KEY_Y                  89
#define EVENT_INPUT_KEY_Z                  90
#define EVENT_INPUT_KEY_LEFT_BRACKET       91  /* [ */
#define EVENT_INPUT_KEY_BACKSLASH          92  /* \ */
#define EVENT_INPUT_KEY_RIGHT_BRACKET      93  /* ] */
#define EVENT_INPUT_KEY_GRAVE_ACCENT       96  /* ` */
#define EVENT_INPUT_KEY_WORLD_1            161 /* non-US #1 */
#define EVENT_INPUT_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define EVENT_INPUT_KEY_ESCAPE             256
#define EVENT_INPUT_KEY_ENTER              257
#define EVENT_INPUT_KEY_TAB                258
#define EVENT_INPUT_KEY_BACKSPACE          259
#define EVENT_INPUT_KEY_INSERT             260
#define EVENT_INPUT_KEY_DELETE             261
#define EVENT_INPUT_KEY_RIGHT              262
#define EVENT_INPUT_KEY_LEFT               263
#define EVENT_INPUT_KEY_DOWN               264
#define EVENT_INPUT_KEY_UP                 265
#define EVENT_INPUT_KEY_PAGE_UP            266
#define EVENT_INPUT_KEY_PAGE_DOWN          267
#define EVENT_INPUT_KEY_HOME               268
#define EVENT_INPUT_KEY_END                269
#define EVENT_INPUT_KEY_CAPS_LOCK          280
#define EVENT_INPUT_KEY_SCROLL_LOCK        281
#define EVENT_INPUT_KEY_NUM_LOCK           282
#define EVENT_INPUT_KEY_PRINT_SCREEN       283
#define EVENT_INPUT_KEY_PAUSE              284
#define EVENT_INPUT_KEY_F1                 290
#define EVENT_INPUT_KEY_F2                 291
#define EVENT_INPUT_KEY_F3                 292
#define EVENT_INPUT_KEY_F4                 293
#define EVENT_INPUT_KEY_F5                 294
#define EVENT_INPUT_KEY_F6                 295
#define EVENT_INPUT_KEY_F7                 296
#define EVENT_INPUT_KEY_F8                 297
#define EVENT_INPUT_KEY_F9                 298
#define EVENT_INPUT_KEY_F10                299
#define EVENT_INPUT_KEY_F11                300
#define EVENT_INPUT_KEY_F12                301
#define EVENT_INPUT_KEY_F13                302
#define EVENT_INPUT_KEY_F14                303
#define EVENT_INPUT_KEY_F15                304
#define EVENT_INPUT_KEY_F16                305
#define EVENT_INPUT_KEY_F17                306
#define EVENT_INPUT_KEY_F18                307
#define EVENT_INPUT_KEY_F19                308
#define EVENT_INPUT_KEY_F20                309
#define EVENT_INPUT_KEY_F21                310
#define EVENT_INPUT_KEY_F22                311
#define EVENT_INPUT_KEY_F23                312
#define EVENT_INPUT_KEY_F24                313
#define EVENT_INPUT_KEY_F25                314
#define EVENT_INPUT_KEY_KP_0               320
#define EVENT_INPUT_KEY_KP_1               321
#define EVENT_INPUT_KEY_KP_2               322
#define EVENT_INPUT_KEY_KP_3               323
#define EVENT_INPUT_KEY_KP_4               324
#define EVENT_INPUT_KEY_KP_5               325
#define EVENT_INPUT_KEY_KP_6               326
#define EVENT_INPUT_KEY_KP_7               327
#define EVENT_INPUT_KEY_KP_8               328
#define EVENT_INPUT_KEY_KP_9               329
#define EVENT_INPUT_KEY_KP_DECIMAL         330
#define EVENT_INPUT_KEY_KP_DIVIDE          331
#define EVENT_INPUT_KEY_KP_MULTIPLY        332
#define EVENT_INPUT_KEY_KP_SUBTRACT        333
#define EVENT_INPUT_KEY_KP_ADD             334
#define EVENT_INPUT_KEY_KP_ENTER           335
#define EVENT_INPUT_KEY_KP_EQUAL           336
#define EVENT_INPUT_KEY_LEFT_SHIFT         340
#define EVENT_INPUT_KEY_LEFT_CONTROL       341
#define EVENT_INPUT_KEY_LEFT_ALT           342
#define EVENT_INPUT_KEY_LEFT_SUPER         343
#define EVENT_INPUT_KEY_RIGHT_SHIFT        344
#define EVENT_INPUT_KEY_RIGHT_CONTROL      345
#define EVENT_INPUT_KEY_RIGHT_ALT          346
#define EVENT_INPUT_KEY_RIGHT_SUPER        347
#define EVENT_INPUT_KEY_MENU               348

#define EVENT_INPUT_KEY_LAST               EVENT_INPUT_KEY_MENU

#endif /* _BRIGHT_INPDEFS_H_ */
