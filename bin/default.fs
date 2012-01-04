#version 330

in vec4 oColor;

out vec4 outColor;

void main() {
    float lerp = 1 - gl_FragCoord.y / 600.f;

    outColor = mix( oColor, vec4( 1.f, 0.f, 0.f, 1.f ), lerp );
}

