/* ======================================================================== */
/* fps_counter.h                                                            */
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
#ifndef _FPS_COUNTER_H_
#define _FPS_COUNTER_H_

#include <chrono>

class FPSCounter {
public:
    FPSCounter()
      : last_time(std::chrono::high_resolution_clock::now())
      { /* do nothing... */ }

    ~FPSCounter() { /* do nothing... */ }

    void update()
      {
        frame_count++;
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = current_time - last_time;

        if (elapsed.count() >= 1.0f) {
            v_fps = frame_count / elapsed.count();
            frame_count = 0;
            last_time = current_time;
        }
      }

    int fps()
      {
        return v_fps;
      }

private:
    int v_fps = 0;
    int frame_count = 0;
    std::chrono::high_resolution_clock::time_point last_time;
};

#endif /* _FPS_COUNTER_H_ */
