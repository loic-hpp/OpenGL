#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in ATTRIB_VS_OUT {
    vec4 color;
    vec2 size;
} attribIn[];

out ATTRIB_GS_OUT {
    vec4 color;
    vec2 texCoords;
} attribOut;

uniform mat4 projection;

void main() {
    // TODO

    vec4 center = gl_in[0].gl_Position;
    vec2 halfSize = attribIn[0].size * 0.5;
    vec4 vertices[4];
    vertices[0] = center + vec4(-halfSize.x, -halfSize.y, 0.0, 0.0);
    vertices[1] = center + vec4(halfSize.x, -halfSize.y, 0.0, 0.0);
    vertices[2] = center + vec4(-halfSize.x, halfSize.y, 0.0, 0.0);
    vertices[3] = center + vec4(halfSize.x, halfSize.y, 0.0, 0.0);

    for(int i = 0; i < 4; ++i) {
        gl_Position = projection * vertices[i];
        attribOut.color = attribIn[0].color;

        // Tex coords
        if(i == 0)
            attribOut.texCoords = vec2(0.0, 0.0);
        else if(i == 1)
            attribOut.texCoords = vec2(1.0, 0.0);
        else if(i == 2)
            attribOut.texCoords = vec2(0.0, 1.0);
        else if(i == 3)
            attribOut.texCoords = vec2(1.0, 1.0);

        EmitVertex();
    }
}
