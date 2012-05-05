#version 120

in vec2 inPosition;
in vec2 inTexcoord;

uniform mat3 ModelMatrix;
uniform mat3 ProjectionMatrix;
uniform int  Depth;

varying vec2 texCoords;
varying vec3 fragPos;

void main() {
    texCoords = inTexcoord;
    fragPos = ModelMatrix * vec3( inPosition, 1.f );

    vec4 oPosition = vec4( ProjectionMatrix * fragPos, 1.f );
    oPosition.z = Depth/10.f;

    gl_Position = oPosition;
}

