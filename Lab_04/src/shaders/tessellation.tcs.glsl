#version 400 core

layout(vertices = 4) out;

uniform mat4 modelView;

const float MIN_TESS = 4;
const float MAX_TESS = 64;

const float MIN_DIST = 30.0;
const float MAX_DIST = 100.0;

void main() {
	// TODO
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    if(gl_InvocationID == 0) {
        float longueurMoyenne = 0.0;
        float distance = 0.0;
        vec4 milieuArete = vec4(0.0);
        for(int i = 0; i < 4; i++) {
            milieuArete = modelView * (gl_in[i].gl_Position + gl_in[(i + 1) % 4].gl_Position) / 2;
            longueurMoyenne = length(milieuArete.xyz);
            distance = clamp((longueurMoyenne - MIN_DIST) / (MAX_DIST - MIN_DIST), 0.0, 1.0);
            gl_TessLevelOuter[i] = mix(MAX_TESS, MIN_TESS, distance);
        }

    // take the max for the inner
        gl_TessLevelInner[0] = gl_TessLevelOuter[1] < gl_TessLevelOuter[3] ? gl_TessLevelOuter[1] : gl_TessLevelOuter[3];
        gl_TessLevelInner[1] = gl_TessLevelOuter[0] < gl_TessLevelOuter[2] ? gl_TessLevelOuter[2] : gl_TessLevelOuter[0];
    }
}
