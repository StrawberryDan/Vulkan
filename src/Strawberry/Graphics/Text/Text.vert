#version 450


vec2 verticies[] = {
        vec2(0.0, 0.0),
        vec2(1.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 1.0)
};


layout(set = 0, binding = 1) uniform PassConstants
{
    mat4 viewMatrix;
};


layout(set = 0, binding = 1) uniform DrawConstants
{
    vec2 position;
    vec2 glyphSize;
};


layout(location = 0) out vec2 texCoords;


void main() {
    gl_Position = vec4(verticies[gl_VertexIndex], 0.0, 1.0);
    texCoords = verticies[gl_VertexIndex];
}
