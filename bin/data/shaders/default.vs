#version 330

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexcoord;

uniform mat3 ModelMatrix;
uniform mat3 ProjectionMatrix;
uniform int  Depth;

out vec2 texCoords;

void main() {
    texCoords = inTexcoord;

    vec4 oPosition = vec4( ProjectionMatrix * ModelMatrix * vec3( inPosition, 1.f ), 1.f );
    oPosition.z = Depth/10.f;

    gl_Position = oPosition;
}

