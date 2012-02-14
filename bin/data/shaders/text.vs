#version 150
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexcoord;

uniform vec2 Position;

out vec2 texCoords;

void main() {
    texCoords = inTexcoord;

    gl_Position = vec4( Position + inPosition, -1, 1 );
}
