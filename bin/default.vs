#version 330

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexcoord;

uniform mat3 ModelMatrix;
uniform mat3 ProjectionMatrix;

//out vec4 oColor;
out vec2 texCoords;

void main() {
    texCoords = inTexcoord;
    gl_Position = vec4( ProjectionMatrix * ModelMatrix * vec3( inPosition, 1.f ), 1.f );
  //  oColor = inColor;
}

