#version 150 core
#define DATA_TYPE_LINE 0
#define DATA_TYPE_DASH 1
#define DATA_TYPE_DASH_DOT 2
#define DATA_TYPE_POINT 3
#define DATA_TYPE_TRIANGLE 4

// Renamed from `texture` so it doesn't shadow the GLSL 1.50 built-in
// `texture()` function. Keeping the old name compiles on some drivers
// (GLSL scoping lets the call resolve by signature) but fails on others.
uniform sampler2D u_texture;

in vec4 v_color;
in vec2 v_position;
in vec2 v_texture;
in vec2 v_start;
in float v_type;

out vec4 fragColor;

const float factor = 2.0;

void main()
{
    int type = int(v_type + 0.1);

    if (type == DATA_TYPE_LINE) {
        fragColor = v_color;
    } else if (type == DATA_TYPE_DASH) {
        vec2 sub = v_position - v_start;
        float coord = length(sub.x) > length(sub.y) ? gl_FragCoord.x : gl_FragCoord.y;
        if (cos(coord / factor) > 0.0) discard;
        fragColor = v_color;
    } else if (type == DATA_TYPE_DASH_DOT) {
        vec2 sub = v_position - v_start;
        float coord = length(sub.x) > length(sub.y) ? gl_FragCoord.x : gl_FragCoord.y;
        float v = cos(coord / factor * 0.5);
        if (v > 0.0 && v < 0.95) discard;
        fragColor = v_color;
    } else if (type == DATA_TYPE_POINT) {
        fragColor = v_color;
    } else if (type == DATA_TYPE_TRIANGLE) {
        fragColor = texture(u_texture, v_texture);
    }
}
