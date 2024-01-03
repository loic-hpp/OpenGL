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
    vec4 Finalcoul = vec4(0.0); 
        vec4 diffuseTex = texture2D(diffuseSampler, attribIn.texCoords);
    if(diffuseTex.a < 0.3)
        discard;
    vec4 specularTex = texture2D(specularSampler, attribIn.texCoords);
    Finalcoul =vec4(((attribIn.ambient + attribIn.diffuse)*diffuseTex.rgb + attribIn.emission + attribIn.specular*specularTex.rgb) ,1.0);
    FragColor = clamp(Finalcoul, 0.0, 1.0);
}
