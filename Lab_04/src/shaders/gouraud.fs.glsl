#version 330 core

in ATTRIB_VS_OUT
{
    vec2 texCoords;
    vec3 emission;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} attribIn;

uniform sampler2D diffuseSampler;
uniform sampler2D specularSampler;

out vec4 FragColor;

void main()
{
    vec4 diffuseTexel = texture(diffuseSampler, attribIn.texCoords);
    if (diffuseTexel.a < 0.3) discard;
    vec4 specularTexel = texture(specularSampler, attribIn.texCoords);

    vec3 color = attribIn.emission + min(attribIn.ambient + attribIn.diffuse, vec3(1.0f)) * vec3(diffuseTexel) + attribIn.specular * vec3(specularTexel.x);
    color = clamp(color, vec3(0.0f), vec3(1.0f));
    FragColor = vec4(color, 1.0f);
}
