#version 460

layout(location = 0) in vec3 position;

layout(push_constant) uniform constants
{
    mat4 MVP;
};

void main() {
    gl_Position = MVP * vec4(position.x, -position.y, position.z, 1.0);
}
