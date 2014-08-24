#version 130

uniform mat4 mvp;

in vec4 vertex_position;
in vec2 vertex_tex_coord;

out vec2 tex_coord;

void main()
{
    tex_coord = vertex_tex_coord;

    gl_Position = mvp * vertex_position;
}
