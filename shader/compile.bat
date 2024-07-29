glslc -fshader-stage=vert ./vertex.glsl -o ./vertex.glsl.spv
glslc -fshader-stage=frag ./fragment.glsl -o ./fragment.glsl.spv

glslc -fshader-stage=vert ./draw_coordinate_axis.vertex.glsl -o ./draw_coordinate_axis.vertex.glsl.spv
glslc -fshader-stage=frag ./draw_coordinate_axis.fragment.glsl -o ./draw_coordinate_axis.fragment.glsl.spv