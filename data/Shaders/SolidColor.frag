#version 460

layout (set=0,binding=0) uniform Block
{
    vec4 color;
};

layout (location=0) out vec4 fragColor;

void main() {
    fragColor = color;
}
