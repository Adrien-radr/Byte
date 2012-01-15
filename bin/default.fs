#version 330

    //in vec4 oColor;

in vec2 texCoords;

out vec4 outColor;

uniform sampler2D Texture;

void main() {
    outColor = texture( Texture, texCoords );
//    outColor += vec4( texCoords.xy, 0.f, 1.f );
}

