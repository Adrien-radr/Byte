#version 330

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

uniform vec2 offset;
uniform mat3 MVP;

out vec4 oColor;

void main() {
    vec3 position = inPosition;

    position.xy += offset;

    gl_Position = vec4( MVP * position, 1.f );
    oColor = inColor;
}
