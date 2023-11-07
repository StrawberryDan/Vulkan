#version 450


vec2 verticies[] = {
        vec2(0.0, 0.0),
        vec2(1.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 1.0)
};


void main() {
    gl_Position = vec4(verticies[gl_VertexIndex], 0.0, 1.0);
}
