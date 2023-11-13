#version 460

layout (location = 0) out vec4 fragColor;

layout (set = 0, binding = 1) uniform sampler2D uTexture;

layout (location = 0) in vec2 texCoords;

void main() {
    fragColor = textureLod(uTexture, texCoords, 0);
}
