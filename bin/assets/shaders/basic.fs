#version 130

uniform sampler2D color_map;
uniform float alpha;

in vec2 tex_coord;

out vec4 frag_color;

void main()
{
    vec4 texel = texture(color_map, tex_coord);

    // since we do not do depth sorting, we need to perform a alpha test
    // https://www.opengl.org/wiki/Transparency_Sorting
    if (texel.a < 0.5) {
        discard;
    }

    frag_color = vec4(texel.rgb, alpha * texel.a);
}
