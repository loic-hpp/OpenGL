#version 330 core
uniform float time;

in ATTRIB_VS_OUT
{
    vec2 texCoords;
    vec3 normal;
    vec3 lightDir[3];
    vec3 spotDir[3];
    vec3 obsPos;
} attribIn;

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

uniform sampler2D diffuseSampler;
uniform sampler2D specularSampler;

out vec4 FragColor;

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
    vec2 texCoords = attribIn.texCoords;
    vec2 timeScale = vec2(1.0, 2.0);
    vec2 offsetScale = vec2(2.0, 2.0);

    vec2 waveFunc = vec2(
        cos(time * timeScale.x + (texCoords.x + texCoords.y) * offsetScale.x),
        sin(time * timeScale.y + (texCoords.x + texCoords.y) * offsetScale.y)
    );

    const vec2 waveAmplitude = vec2(0.025, 0.01);
    vec2 waveOffset =  waveFunc*waveAmplitude;

    texCoords += waveOffset;

    vec4 diffuseTexel = texture(diffuseSampler, texCoords);
    if (diffuseTexel.a < 0.3) discard;

    vec4 specularTexel = texture(specularSampler, texCoords);

    vec3 n = normalize(attribIn.normal);
    vec3 ldir0 = normalize(attribIn.lightDir[0]);
    vec3 ldir1 = normalize(attribIn.lightDir[1]);
    vec3 ldir2 = normalize(attribIn.lightDir[2]);
    vec3 op = normalize(attribIn.obsPos);

    float spotFactor0 = max(int(!useSpotlight), computeSpot(normalize(attribIn.spotDir[0]), ldir0, n));
    float spotFactor1 = max(int(!useSpotlight), computeSpot(normalize(attribIn.spotDir[1]), ldir1, n));
    float spotFactor2 = max(int(!useSpotlight), computeSpot(normalize(attribIn.spotDir[2]), ldir2, n));

    vec3 emission = mat.emission;    
    vec3 ambient = lights[0].ambient * mat.ambient
                 + lights[1].ambient * mat.ambient
                 + lights[2].ambient * mat.ambient;

    ambient += lightModelAmbient * mat.ambient;

    vec3 lightDiffuse[3];
    vec3 lightSpecular[3];

    computeLight(0, n, ldir0, op, lightDiffuse[0], lightSpecular[0]);
    computeLight(1, n, ldir1, op, lightDiffuse[1], lightSpecular[1]);
    computeLight(2, n, ldir2, op, lightDiffuse[2], lightSpecular[2]);

    vec3 diffuse = lightDiffuse[0] * spotFactor0 + lightDiffuse[1] * spotFactor1 + lightDiffuse[2] * spotFactor2;
    vec3 specular = lightSpecular[0] * spotFactor0 + lightSpecular[1] * spotFactor1 + lightSpecular[2] * spotFactor2;

    vec3 color = emission + (ambient + diffuse) * vec3(diffuseTexel) + specular * vec3(specularTexel.x);

    color = clamp(color, vec3(0.0f), vec3(1.0f));
    FragColor = vec4(color, 1.0f);
}