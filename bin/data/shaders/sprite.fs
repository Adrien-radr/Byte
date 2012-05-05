#version 120

varying vec2 texCoords;
varying vec2 spritePos;     

// TEXTURE
uniform sampler2D   Albedo;
uniform sampler2D   Heightmap;

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
    //if( 0.6 > oColor.a )
    //    discard;

    // get heightmap greyscale color to approximate fragment height on sprite
    float fragHeight = 255 * texture2D( Heightmap, texCoords ).r;

    // get direction from frag pos to light
    vec3 light_dir = vec3( light_pos, light_height ) - vec3( spritePos, fragHeight );

    // attenuation
    float d = length( light_dir );
    float att = 1.f / ( light_cstatt + light_linatt * d + light_quadatt * d * d );


    // compute lambert term for diffuse reflection
    vec3 L = light_dir / d;
    vec3 N = L;
    float lambert_term = max( 0.f, dot( N,L ) );

    vec4 illum = amb_color + ( light_color * lambert_term * att );
    vec4 tex_color = texture2D( Albedo, texCoords );

    // mix the texture and illum
    gl_FragColor = vec4( illum.rgb, 1.f ) * tex_color;
}

