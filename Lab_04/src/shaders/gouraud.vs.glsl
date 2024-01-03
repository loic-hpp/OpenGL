#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out ATTRIB_VS_OUT
{
    vec2 texCoords;
    vec3 emission;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} attribOut;

uniform mat4 mvp;
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
    vec4 modelPos = vec4(position.xyz, 1.0);
    gl_Position = mvp * modelPos;
    attribOut.texCoords = texCoords;

    vec3 n = normal;
    if (normal.x == 0 && normal.y == 0 && normal.z == 0)
        n = vec3(0,1,0);
    vec3 normal = normalize(normalMatrix * n);

    vec4 viewPos = modelView * modelPos;
    vec3 lightDir0 = normalize(( view * vec4(lights[0].position, 1.0f) ).xyz - viewPos.xyz);
    vec3 lightDir1 = normalize(( view * vec4(lights[1].position, 1.0f) ).xyz - viewPos.xyz);
    vec3 lightDir2 = normalize(( view * vec4(lights[2].position, 1.0f) ).xyz - viewPos.xyz);
    vec3 obsPos = normalize(-viewPos.xyz);
    
    vec3 spotDir0 = normalize(mat3(view) * -lights[0].spotDirection);
    vec3 spotDir1 = normalize(mat3(view) * -lights[1].spotDirection);
    vec3 spotDir2 = normalize(mat3(view) * -lights[2].spotDirection);

    float spotFactor0 = max(int(!useSpotlight), computeSpot(spotDir0, lightDir0, normal));
    float spotFactor1 = max(int(!useSpotlight), computeSpot(spotDir1, lightDir1, normal));
    float spotFactor2 = max(int(!useSpotlight), computeSpot(spotDir2, lightDir2, normal));

    attribOut.emission = mat.emission;    
    attribOut.ambient = lightModelAmbient * mat.ambient
                      + lights[0].ambient * mat.ambient
                      + lights[1].ambient * mat.ambient
                      + lights[2].ambient * mat.ambient;
    
    vec3 lightDiffuse[3];
    vec3 lightSpecular[3];

    computeLight(0, normal, lightDir0, obsPos, lightDiffuse[0], lightSpecular[0]);
    computeLight(1, normal, lightDir1, obsPos, lightDiffuse[1], lightSpecular[1]);
    computeLight(2, normal, lightDir2, obsPos, lightDiffuse[2], lightSpecular[2]);

    attribOut.diffuse = lightDiffuse[0] * spotFactor0 + lightDiffuse[1] * spotFactor1 + lightDiffuse[2] * spotFactor2;
    attribOut.specular = lightSpecular[0] * spotFactor0 + lightSpecular[1] * spotFactor1 + lightSpecular[2] * spotFactor2;
}
