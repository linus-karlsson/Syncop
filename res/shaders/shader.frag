#version 450 core

layout(location = 0) in vec4 f_color;

layout(location = 0) out vec4 out_color;

void main(void)
{
    out_color = f_color;
}
