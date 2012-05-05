#version 120

in vec2 inPosition;
in vec2 inTexcoord;

uniform mat3 ModelMatrix;       // contains the sprite half-height in [0][2]
uniform mat3 ProjectionMatrix;
uniform int  Depth;

varying vec2 texCoords;
varying vec2 spritePos;     // global position of sprite on map


void main() {
    mat3 MM = ModelMatrix;

    // Sprite Pos on map : The Translation component of the ModelMatrix,
    // minus the half height of the sprite (on component y)
    spritePos = MM[2].xy;
    spritePos.y += MM[0].z;

    // remove the half-height component from model matrix
    MM[0].z = 0.f;


    // projected and correctly positionned vertex on screen
    vec4 oPosition = vec4( ProjectionMatrix * MM * vec3( inPosition, 1.f ), 1.f );

    // set the depth of the vertex (2d layers)
    oPosition.z = Depth/10.f;

    texCoords = inTexcoord;
    gl_Position = oPosition;
}

