#version 120

varying vec2 texCoords;
varying vec3 fragPos;


// TEXTURE
uniform sampler2D   Albedo;

// AMBIENT LIGHT
uniform vec4        amb_color;

// DIFFUSE LIGHT
uniform vec4        light_color;
uniform vec2        light_pos;
uniform float       light_height;
uniform float       light_cstatt;
uniform float       light_linatt;
uniform float       light_quadatt;



void main() {
    // get the fragment position in isometric space depending on the light position. Then get the light direction from that iso point.
    vec2 iso_pos = ((fragPos.xy - light_pos) * vec2( 1.f, 2.f )) + light_pos;
    vec3 light_dir = vec3( light_pos, light_height ) - vec3( iso_pos, 0.f );

    // attenuation
    float d = length( light_dir );
    float att = 1.f / ( light_cstatt + light_linatt * d + light_quadatt * d * d );


    // compute lambert term for diffuse reflection
    vec3 L = light_dir / d;
    vec3 N = vec3( 0, 0, 1 );
    float lambert_term = max( 0.f, dot( N, L ) );


    vec4 illum = amb_color + (light_color * lambert_term * att);
    vec4 tex_color = texture2D( Albedo, texCoords );

    // mix the texture and the illumination
    gl_FragColor = vec4( illum.rgb, 1.f ) * tex_color;
}

