#version 460

layout (location = 0) out vec4 fragColor;

layout (set = 1, binding = 0) uniform sampler2D uTexture;

layout (location = 0) in vec2 texCoords;

void main() {
    fragColor = textureLod(uTexture, texCoords, 0);
}
