#version 120

in vec2 inPosition;
in vec2 inTexcoord;

uniform vec2 Position;
uniform mat3 ProjectionMatrix;
uniform int  Depth;

varying vec2 texCoords;

void main() {
    texCoords = inTexcoord;

    vec4 oPosition = vec4( ProjectionMatrix * vec3( inPosition + Position, 1 ), 1 );
    oPosition.z = Depth / 10.f;

    gl_Position = oPosition;
}
