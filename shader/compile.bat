@REM /* ======================================================================== */
@REM /* compile.bat                                                              */
@REM /* ======================================================================== */
@REM /*                        This file is part of:                             */
@REM /*                           COPILOT ENGINE                                 */
@REM /* ======================================================================== */
@REM /*                                                                          */
@REM /* Copyright (C) 2022 Vcredent All rights reserved.                         */
@REM /*                                                                          */
@REM /* Licensed under the Apache License, Version 2.0 (the "License");          */
@REM /* you may not use this file except in compliance with the License.         */
@REM /*                                                                          */
@REM /* You may obtain a copy of the License at                                  */
@REM /*     http://www.apache.org/licenses/LICENSE-2.0                           */
@REM /*                                                                          */
@REM /* Unless required by applicable law or agreed to in writing, software      */
@REM /* distributed under the License is distributed on an "AS IS" BASIS,        */
@REM /* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, e1ither express or implied */
@REM /* See the License for the specific language governing permissions and      */
@REM /* limitations under the License.                                           */
@REM /*                                                                          */
@REM /* ======================================================================== */
@echo off

@REM ----------------------- MER@
@REM       graph shader      MER@
@REM ----------------------- MER@
glslc -fshader-stage=vert ./graph_vertex.glsl -o ./graph_vertex.spv
glslc -fshader-stage=frag ./graph_fragment.glsl -o ./graph_fragment.spv

@REM ----------------------------- MER@
@REM     coordinate line shader    MER@
@REM ----------------------------- MER@
glslc -fshader-stage=vert ./coordinate_axis_vertex.glsl -o ./coordinate_axis_vertex.spv
glslc -fshader-stage=frag ./coordinate_axis_fragment.glsl -o ./coordinate_axis_fragment.spv