#version 450 core

layout(location = 0) in vec4 v_color;
layout(location = 1) in vec2 v_position;
layout(location = 2) in vec2 v_texture_coordinate;
layout(location = 3) in float v_texture_index;

layout(location = 0) out vec4 f_color;
layout(location = 1) out vec2 f_texture_coordinate;
layout(location = 2) out flat float f_texture_index;

layout(binding = 0) uniform ViewProjection
{
    mat4 view;
    mat4 projection;
}
VP;

layout(push_constant) uniform Model
{
    mat4 model;
}
M;

void main(void)
{
    gl_Position = VP.projection * VP.view * M.model * vec4(v_position, 0.0, 1.0);
    // gl_Position = vec4(v_position, 0.0, 1.0);

    f_color = v_color;
    f_texture_coordinate = v_texture_coordinate;
    f_texture_index = v_texture_index;
}
