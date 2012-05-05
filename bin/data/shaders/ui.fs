#version 120

varying vec2 texCoords;

uniform sampler2D   Albedo;
uniform vec4        Color;

void main() {
    gl_FragColor = texture2D( Albedo, texCoords );
}
