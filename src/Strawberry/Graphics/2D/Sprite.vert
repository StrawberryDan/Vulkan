#version 460

layout (set=0, binding=0) uniform CameraBuffer
{
    mat4 viewMatrix;
};

layout (push_constant) uniform Contants
{
    mat4 modelMatrix;
};

layout (location = 0) out vec2 texCoords;

vec2 positions[] = {
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),

    vec2(1.0, 0.0),
    vec2(1.0, 1.0),
    vec2(0.0, 1.0)
};

void main() {
    gl_Position = viewMatrix * modelMatrix * vec4(positions[gl_VertexIndex].xy, 0.0, 1.0);
    texCoords = positions[gl_VertexIndex].xy;
}
