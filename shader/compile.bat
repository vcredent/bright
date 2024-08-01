glslc -fshader-stage=vert ./vertex.glsl -o ./vertex.glsl.spv
glslc -fshader-stage=frag ./fragment.glsl -o ./fragment.glsl.spv

glslc -fshader-stage=vert ./axis_line_vertex.glsl -o ./axis_line_vertex.glsl.spv
glslc -fshader-stage=frag ./axis_line_fragment.glsl -o ./axis_line_fragment.glsl.spv