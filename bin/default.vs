#version 330

layout(location = 0) in vec2 inPosition;
//layout(location = 1) in vec4 inColor;

uniform mat3 ModelMatrix;
uniform mat3 ProjectionMatrix;

//out vec4 oColor;

void main() {
    gl_Position = vec4( ProjectionMatrix * ModelMatrix * vec3( inPosition, 1.f ), 1.f );
  //  oColor = inColor;
}

