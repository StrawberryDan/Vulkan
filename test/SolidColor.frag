#version 460

layout (location=0) out vec4 fragColor;

layout (push_constant) uniform constants
{
    layout (offset = 64) vec3 color;
};

void main() {
    fragColor = vec4(color, 1.0);
}
