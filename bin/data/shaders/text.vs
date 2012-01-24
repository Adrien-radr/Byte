#version 330

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexcoord;

out vec2 texCoords;

void main() {
    texCoords = inTexcoord;
    gl_Position = vec4( inPosition, 0, 1 );
}
