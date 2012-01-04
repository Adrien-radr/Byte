#version 330

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;

uniform vec2 offset;

out vec4 oColor;

void main() {
    vec4 realOffset = vec4( offset.x, offset.y, 0.f, 0.f );

    gl_Position = inPosition + realOffset;
    oColor = inColor;
}
