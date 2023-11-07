#version 450


layout(set = 0,binding = 2) uniform sampler2D mSampler;


layout(location = 0) in vec2 texCoords;


layout(location=0) out vec4 fragColor;

void main() {
    fragColor = texture(mSampler, texCoords);
}
