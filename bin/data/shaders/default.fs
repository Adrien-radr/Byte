#version 330

in vec2 texCoords;

out vec4 outColor;

uniform sampler2D Albedo;

void main() {
    outColor = texture( Albedo, texCoords );
}

