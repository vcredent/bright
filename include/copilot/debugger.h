/* ======================================================================== */
/* debugger.h                                                              */
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
#ifndef _ENGINE_DEBUGGER_H_
#define _ENGINE_DEBUGGER_H_

#include <copilot/typedefs.h>
#include <copilot/memalloc.h>

struct DebuggerProperties {
    int   fps                       = 0;
    float scene_render_time         = 0.0f;
    float screen_render_time        = 0.0f;
};

namespace Debugger
{
    extern DebuggerProperties *v_debugger_properties;

V_FORCEINLINE static void set_fps_value(int fps)
  {
      v_debugger_properties->fps = fps;
  }

V_FORCEINLINE static void set_scene_render_time_value(float time)
  {
      v_debugger_properties->scene_render_time = time;
  }

V_FORCEINLINE static void set_screen_render_time(float time)
  {
      v_debugger_properties->screen_render_time = time;
  }
}

#endif /* _ENGINE_DEBUGGER_H_ */
