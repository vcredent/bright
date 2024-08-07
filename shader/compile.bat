:: ======================================================================== ::
:: compile.bat                                                              ::
:: ======================================================================== ::
::                        This file is part of:                             ::
::                            BRIGHT ENGINE                                 ::
:: ======================================================================== ::
::                                                                          ::
:: Copyright (C) 2022 Vcredent All rights reserved.                         ::
::                                                                          ::
:: Licensed under the Apache License, Version 2.0 (the "License");          ::
:: you may not use this file except in compliance with the License.         ::
::                                                                          ::
:: You may obtain a copy of the License at                                  ::
::     http://www.apache.org/licenses/LICENSE-2.0                           ::
::                                                                          ::
:: Unless required by applicable law or agreed to in writing, software      ::
:: distributed under the License is distributed on an "AS IS" BASIS,        ::
:: WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, e1ither express or implied ::
:: See the License for the specific language governing permissions and      ::
:: limitations under the License.                                           ::
::                                                                          ::
:: ======================================================================== ::
@echo off
setlocal

call :makeall graph
call :makeall coordaxis
call :makeall sky_sphere

exit

:makevert
glslc -fshader-stage=vert ./%1.vert -o ./%1.vert.spv
goto :eof

:makefrag
glslc -fshader-stage=frag ./%1.frag -o ./%1.frag.spv
goto :eof

:makeall
call :makevert %1
call :makefrag %1
goto :eof

endlocal