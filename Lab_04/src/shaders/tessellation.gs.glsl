#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in ATTRIB_TES_OUT {
    float height;
    vec2 texCoords;
    vec4 patchDistance;
} attribIn[];

out ATTRIB_GS_OUT {
    float height;
    vec3 barycentricCoords;
    vec2 texCoords;
    vec4 patchDistance;
} attribOut;

void main() {
    // TODO
    vec3 barycentricCoords[3];
    barycentricCoords[0] = vec3(1.0, 0.0, 0.0);
    barycentricCoords[1] = vec3(0.0, 1.0, 0.0);
    barycentricCoords[2] = vec3(0.0, 0.0, 1.0);

    for(int i = 0; i < gl_in.length(); ++i) {
        gl_Position = gl_in[i].gl_Position;
        attribOut.barycentricCoords = barycentricCoords[i];
        attribOut.height = attribIn[i].height;
        attribOut.patchDistance = attribIn[i].patchDistance;
        attribOut.texCoords = attribIn[i].texCoords;
        EmitVertex();
    }
}
