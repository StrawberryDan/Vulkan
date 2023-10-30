#version 460

layout(location = 0) in vec3 position;

layout (location = 0) out vec3 outPosition;

layout(push_constant) uniform constants
{
    mat4 MVP;
};

void main() {
    gl_Position = MVP * vec4(position.x, -position.y, position.z, 1.0);
    outPosition = gl_Position.xyz;
}
