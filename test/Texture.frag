#version 460

layout (location=0) out vec4 fragColor;

layout (push_constant) uniform constants
{
    layout (offset = 64) vec3 color;
};

layout (set = 0, binding = 0) uniform sampler2D textureSampler;


layout (location = 0) in vec3 position;


void main() {
    fragColor = texture(textureSampler, position.xy);
}
