#version 130

uniform mat4 model_view;
uniform mat4 projection;

uniform vec3 scale;
uniform bool billboarding;
uniform bool spherical;
uniform bool cheap_method;

in vec4 vertex_position;
in vec2 vertex_tex_coord;

out vec4 position;
out vec2 tex_coord;

void main()
{
    tex_coord = vertex_tex_coord;

    mat4 billboard_mv = model_view;
    if (billboarding && cheap_method) {
        billboard_mv[0][0] = scale.x;
        billboard_mv[0][1] = 0.0;
        billboard_mv[0][2] = 0.0;
        if (spherical) {
            billboard_mv[1][0] = 0.0;
            billboard_mv[1][1] = scale.y;
            billboard_mv[1][2] = 0.0;
        }
        billboard_mv[2][0] = 0.0;
        billboard_mv[2][1] = 0.0;
        billboard_mv[2][2] = scale.z;
    }

    position = projection * billboard_mv * vertex_position;
    gl_Position = position;
}
