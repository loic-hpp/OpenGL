#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in ATTRIB_OUT
{
    vec3 position;
    vec2 texCoords;
} attribIn[];

out ATTRIB_VS_OUT
{
    vec2 texCoords;    
    vec3 emission;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} attribOut;

uniform mat4 view;
uniform mat4 modelView;
uniform mat3 normalMatrix;

struct Material
{
    vec3 emission;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct UniversalLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    vec3 spotDirection;
};

layout (std140) uniform LightingBlock
{
    Material mat;
    UniversalLight lights[3];
    vec3 lightModelAmbient;
    bool useBlinn;
    bool useSpotlight;
    bool useDirect3D;
    float spotExponent;
    float spotOpeningAngle;
};

float computeSpot(in vec3 spotDir, in vec3 lightDir, in vec3 normal)
{
    float spotFactor = 0.0;
    
    if (dot(spotDir, normal) >= 0.0)
    {
        float spotDot = dot(lightDir, spotDir);
        float opening = cos(radians(spotOpeningAngle));
        
        if (useDirect3D)
            spotFactor = smoothstep(pow(opening, 1.01 + spotExponent/2.0), opening, spotDot);
        else
            spotFactor = spotDot > opening ? pow(spotDot, spotExponent) : 0.0;            
    }
    return spotFactor;
}

void computeLight(in int lightIndex, in vec3 normal, in vec3 lightDir, in vec3 obsPos, out vec3 diffuseColor, out vec3 specularColor)
{
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lights[lightIndex].diffuse * mat.diffuse;
    
    float spec = useBlinn ? 
                 dot(normalize( lightDir + obsPos ), normal) :
                 dot(reflect(-lightDir, normal), obsPos);
    spec = max(0.0, spec);
    spec = pow(spec, mat.shininess);
    vec3 specular = spec * lights[lightIndex].specular * mat.specular;

    diffuseColor = diffuse;
    specularColor = specular;
}


void main()
{
    vec3 side1 = (attribIn[1].position - attribIn[0].position).xyz;
    vec3 side2 = (attribIn[2].position - attribIn[0].position).xyz;
    vec3 normal = cross(side1, side2);

    vec3 faceCenterPos = (attribIn[0].position + attribIn[1].position + attribIn[2].position) / 3.0f;
    
    vec3 n = normalize(normalMatrix * normal);

    vec4 viewPos = modelView * vec4(faceCenterPos, 1.0f);
    vec3 lightDir0 = normalize(( view * vec4(lights[0].position, 1.0f)).xyz - viewPos.xyz);
    vec3 lightDir1 = normalize(( view * vec4(lights[1].position, 1.0f)).xyz - viewPos.xyz);
    vec3 lightDir2 = normalize(( view * vec4(lights[2].position, 1.0f)).xyz - viewPos.xyz);
    vec3 obsPos = normalize(-viewPos.xyz);

    vec3 spotDir0 = normalize(mat3(view) * -lights[0].spotDirection);
    vec3 spotDir1 = normalize(mat3(view) * -lights[1].spotDirection);
    vec3 spotDir2 = normalize(mat3(view) * -lights[2].spotDirection);

    float spotFactor0 = max(int(!useSpotlight), computeSpot(spotDir0, lightDir0, n));
    float spotFactor1 = max(int(!useSpotlight), computeSpot(spotDir1, lightDir1, n));
    float spotFactor2 = max(int(!useSpotlight), computeSpot(spotDir2, lightDir2, n));

    vec3 emission = mat.emission;    
    vec3 ambient = lightModelAmbient * mat.ambient
                 + lights[0].ambient * mat.ambient
                 + lights[1].ambient * mat.ambient
                 + lights[2].ambient * mat.ambient;

    vec3 lightDiffuse[3];
    vec3 lightSpecular[3];

    computeLight(0, n, lightDir0, obsPos, lightDiffuse[0], lightSpecular[0]);
    computeLight(1, n, lightDir1, obsPos, lightDiffuse[1], lightSpecular[1]);
    computeLight(2, n, lightDir2, obsPos, lightDiffuse[2], lightSpecular[2]);

    vec3 diffuse = lightDiffuse[0] * spotFactor0 + lightDiffuse[1] * spotFactor1 + lightDiffuse[2] * spotFactor2;
    vec3 specular = lightSpecular[0] * spotFactor0 + lightSpecular[1] * spotFactor1 + lightSpecular[2] * spotFactor2;

    for (int i = 0; i < gl_in.length(); ++i)
    {
        gl_Position = gl_in[i].gl_Position;
        attribOut.texCoords = attribIn[i].texCoords;
        attribOut.emission = emission;
        attribOut.ambient = ambient;
        attribOut.diffuse = diffuse;
        attribOut.specular = specular;
        EmitVertex();
    }
    EndPrimitive();
}
