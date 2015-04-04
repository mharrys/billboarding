#version 130

uniform sampler2D map;
uniform vec4 diffuse;

in vec4 position;
in vec2 tex_coord;

out vec4 frag_color;

void main()
{
    vec4 texel = texture(map, tex_coord);

    // since we do not do depth sorting, we need to perform a alpha test
    // https://www.opengl.org/wiki/Transparency_Sorting
    if (texel.a < 0.5) {
        discard;
    }

    float z = position.z;
    float dim = 1.0 / (1.0 + (z * 0.01) + (z * z * 0.001));

    frag_color = vec4(texel.rgb * diffuse.rgb * dim, diffuse.a * texel.a);
}
