#version 330 core

in ATTRIB_GS_OUT {
    float height;
    vec3 barycentricCoords;
    vec2 texCoords;
    vec4 patchDistance;
} attribIn;

uniform sampler2D groundSampler;
uniform sampler2D sandSampler;
uniform sampler2D snowSampler;
uniform bool viewWireframe;

out vec4 FragColor;

float edgeFactor(vec3 barycentricCoords, float width) {
    vec3 d = fwidth(barycentricCoords);
    vec3 f = step(d * width, barycentricCoords);
    return min(min(f.x, f.y), f.z);
}

float edgeFactor(vec4 barycentricCoords, float width) {
    vec4 d = fwidth(barycentricCoords);
    vec4 f = step(d * width, barycentricCoords);
    return min(min(min(f.x, f.y), f.z), f.w);
}

const vec3 WIREFRAME_COLOR = vec3(0.5);
const vec3 PATCH_EDGE_COLOR = vec3(1.0, 0.0, 0.0);

const float WIREFRAME_WIDTH = 0.5;
const float PATCH_EDGE_WIDTH = 0.5;

void main() {
	// TODO
    vec4 groundColor = texture(groundSampler, attribIn.texCoords);
    vec4 sandColor = texture(sandSampler, attribIn.texCoords);
    vec4 snowColor = texture(snowSampler, attribIn.texCoords);

    float height = attribIn.height;

    vec4 texColor;

    if(height >= 0 && height < 0.3) {
        texColor = sandColor;
    } else if(height >= 0.3 && height < 0.35) {
        texColor = mix(sandColor, groundColor, smoothstep(0.3, 0.35, height));
    } else if(height >= 0.35 && height < 0.6) {
        texColor = groundColor;
    } else if(height >= 0.6 && height < 0.65) {
        texColor = mix(groundColor, snowColor, smoothstep(0.6, 0.65, height));
    } else {
        texColor = snowColor;
    }

    vec3 frameColor = texColor.rgb;
    float edgeFactorWireFrame = edgeFactor(attribIn.barycentricCoords, WIREFRAME_WIDTH);
    frameColor = mix(WIREFRAME_COLOR, frameColor, edgeFactorWireFrame);
    float edgeFactorPatch = edgeFactor(attribIn.patchDistance, PATCH_EDGE_WIDTH);
    frameColor = mix(PATCH_EDGE_COLOR, frameColor, edgeFactorPatch);

    if(viewWireframe) {
        FragColor = vec4(frameColor.rgb, 1.0);
    } else {
        FragColor = vec4(texColor.rgb, 1.0);
    }

}
