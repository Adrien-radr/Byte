#version 150
#extension GL_ARB_separate_shader_objects : enable

in vec2 texCoords;

out vec4 outColor;

uniform sampler2D   Albedo;
uniform vec4        Color;

void main() {
    outColor = vec4( 1, 1, 1, texture( Albedo, texCoords ).r ) * Color;
}
