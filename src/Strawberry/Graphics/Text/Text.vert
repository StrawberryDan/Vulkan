#version 450


vec2 verticies[] = {
        vec2(0.0, 1.0),
        vec2(1.0, 1.0),
        vec2(0.0, 0.0),
        vec2(1.0, 0.0)
};


layout(set = 0, binding = 0) uniform PassConstants
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
    gl_Position = viewMatrix * vec4(glyphSize * verticies[gl_VertexIndex] + position, 0.0, 1.0);
    texCoords = verticies[gl_VertexIndex];
}
