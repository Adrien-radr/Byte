#version 330

in vec2 texCoords;

out vec4 outColor;

uniform sampler2D Albedo;

void main() {
    outColor = texture( Albedo, texCoords );
//    outColor += vec4( texCoords.xy, 0.f, 1.f );
}

