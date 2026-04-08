#version 130
#define DATA_TYPE_LINE 0
#define DATA_TYPE_DASH 1
#define DATA_TYPE_DASH_DOT 2
#define DATA_TYPE_POINT 3
#define DATA_TYPE_TRIANGLE 4

uniform mat4 p_matrix;
uniform mat4 v_matrix;
uniform mat4 m_matrix;

in vec4 a_position;
in vec4 a_color;
in vec4 a_data;
in float a_type;

out vec2 v_position;
out vec4 v_color;
out vec2 v_texture;
out float v_type;
out vec2 v_start;

void main()
{
    mat4 mvp_matrix = p_matrix * v_matrix * m_matrix;

    v_type = a_type;
    int type = int(v_type + 0.1);

    if (type == DATA_TYPE_LINE) {
    } else if (type == DATA_TYPE_DASH) {
        v_start = (mvp_matrix * a_data).xy;
        v_position = (mvp_matrix * a_position).xy;
    } else if (type == DATA_TYPE_DASH_DOT) {
        v_start = (mvp_matrix * a_data).xy;
        v_position = (mvp_matrix * a_position).xy;
    } else if (type == DATA_TYPE_POINT) {
        gl_PointSize = a_data.x;
    } else if (type == DATA_TYPE_TRIANGLE) {
        v_texture = a_data.yz;
    }

    v_color = a_color;
    gl_Position = mvp_matrix * a_position;
}
