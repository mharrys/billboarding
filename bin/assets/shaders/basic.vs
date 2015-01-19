#version 130

uniform mat4 model_view;
uniform mat4 projection;

in vec4 vertex_position;
in vec2 vertex_tex_coord;

out vec4 position;
out vec2 tex_coord;

void main()
{
    position = vertex_position;
    tex_coord = vertex_tex_coord;

    gl_Position = projection * model_view * vertex_position;
}
