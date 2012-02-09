#version 150
#extension GL_ARB_explicit_attrib_location :  enable

in vec2 texCoords;

out vec4 outColor;

uniform sampler2D Albedo;

void main() {
    outColor = texture( Albedo, texCoords );
}

