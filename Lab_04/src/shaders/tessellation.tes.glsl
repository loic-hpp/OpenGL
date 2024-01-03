#version 400 core

layout(quads) in;

/*
in Attribs {
    vec4 couleur;
} AttribsIn[];*/


out ATTRIB_TES_OUT
{
    float height;
    vec2 texCoords;
    vec4 patchDistance;
} attribOut;

uniform mat4 mvp;

uniform sampler2D heighmapSampler;

vec4 interpole( vec4 v0, vec4 v1, vec4 v2, vec4 v3 )
{
    // mix( x, y, f ) = x * (1-f) + y * f.
    // TODO
    vec4 v01 = mix( v0, v1, gl_TessCoord.x );
    vec4 v32 = mix( v3, v2, gl_TessCoord.x );
    return mix( v01, v32, gl_TessCoord.y );
}


const float PLANE_SIZE = 256.0f;

void main()
{
	// TODO
    vec4 position = interpole( gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position );
    vec2 texCoord = ((position + PLANE_SIZE/2)/(4*PLANE_SIZE)).xz;
    float heightValue = texture(heighmapSampler, texCoord).r;
    attribOut.height = heightValue;
    attribOut.patchDistance = vec4(gl_TessCoord.x, 1.0-gl_TessCoord.x, gl_TessCoord.y, 1.0  -gl_TessCoord.y);
    attribOut.texCoords = 2*gl_TessCoord.xy;
    
    position.y = mix(-32.0, 32.0, heightValue);
    gl_Position = mvp * position;
}
