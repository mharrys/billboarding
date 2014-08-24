#version 130

uniform mat4 mvp;

in vec4 vertex_position;
in vec4 vertex_color;

out vec4 color;

void main()
{
    color = vertex_color;
    gl_Position = mvp * vertex_position;
}
