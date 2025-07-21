#version 460 core

layout(location=0) in vec3 a_location;
layout(location=1) in vec2 a_texture;

out vec2 v_texture;

void main() {
    gl_Position = vec4(a_location, 1.0);
    v_texture = a_texture;
}
