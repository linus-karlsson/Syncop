#version 450 core

layout(location = 0) in vec4 f_color;
layout(location = 1) in vec2 f_texture_coordinate;
layout(location = 2) in flat float f_texture_index;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 1) uniform sampler2D textures[2];

void main(void)
{
    int index = int(f_texture_index);
    vec4 f_texture = texture(textures[index], f_texture_coordinate) * f_color;
    out_color = f_texture;
}
