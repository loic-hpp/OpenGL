#version 330 core

in ATTRIB_VS_OUT {
    vec2 texCoords;
    vec3 normal;
    vec3 lightDir[3];
    vec3 spotDir[3];
    vec3 obsPos;
} attribIn;

struct Material {
    vec3 emission;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct UniversalLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    vec3 spotDirection;
};

layout(std140) uniform LightingBlock {
    Material mat;
    UniversalLight lights[3];
    vec3 lightModelAmbient;
    bool useBlinn;
    bool useSpotlight;
    bool useDirect3D;
    float spotExponent;
    float spotOpeningAngle;
} lightSource;

uniform sampler2D diffuseSampler;
uniform sampler2D specularSampler;

out vec4 FragColor;

vec4 diffuseTex = texture2D(diffuseSampler, attribIn.texCoords);
vec4 specularTex = texture2D(specularSampler, attribIn.texCoords);

float computeSpot(in vec3 spotDir, in vec3 lightDir, in vec3 normal) {
    vec3 N = normalize(normal); // vecteur normal
    vec3 L = normalize(lightDir); // vecteur vers la lumière
    vec3 D = normalize(spotDir);
    float spotFacteur = 0.0;
    if(lightSource.useSpotlight) {
        if(dot(D, N) >= 0) {
            float spotDot = dot(L, D);
            spotFacteur = (spotDot > cos(radians(lightSource.spotOpeningAngle))) ? pow(spotDot, lightSource.spotExponent) : 0.0;
            if(spotDot > cos(radians(lightSource.spotOpeningAngle)))
                spotFacteur = pow(spotDot, lightSource.spotExponent);
        }

        if(lightSource.useDirect3D) {
            float cosDelta = cos(radians(lightSource.spotOpeningAngle));
            float cosOuter = pow(cosDelta, 1.01 + (lightSource.spotExponent / 2));
            float cosInner = cosDelta;
            float cosGamma = dot(L, D);
            spotFacteur = smoothstep(cosOuter, cosInner, cosGamma);
        }
    } else
        spotFacteur = 1.0;
    return spotFacteur;
}

vec3 computeLight(in int lightIndex, in vec3 normal, in vec3 lightDir, in vec3 obsPos) {
    vec3 coul = vec3(0);
    vec3 N = normalize(normal); // vecteur normal
    vec3 L = normalize(lightDir); // vecteur vers la lumière
    vec3 O = normalize(obsPos); // position de l'observateur
    float attenuation = 1.0;
    if(diffuseTex.a < 0.3)
        discard;
    float d = length(L);
    coul += diffuseTex.rgb * vec3((lightSource.mat.ambient * lightSource.lights[lightIndex].ambient));

    float NdotL = max(0.0, dot(N, L));
    if(NdotL > 0.0) {
        // calculer la composante diffuse
        coul += diffuseTex.rgb * attenuation * lightSource.mat.diffuse * lightSource.lights[lightIndex].diffuse * NdotL;

        // calculer la composante spéculaire (Blinn ou Phong : spec = BdotN ou RdotO )
        float spec = (lightSource.useBlinn ? dot(normalize(L + O), N) : // dot( B, N )
        dot(reflect(-L, N), O)); // dot( R, O )
        if(spec > 0)
            coul += specularTex.rgb * attenuation * lightSource.mat.specular * lightSource.lights[lightIndex].specular * pow(spec, lightSource.mat.shininess);
    }
    return (coul);
}

void main() {
    vec4 coul[3];
    float spotCalcul;
    vec3 reflexion;
        // Calcul de l'émission et du terme ambiant
    vec4 Finalcoul = vec4((lightSource.mat.emission + lightSource.mat.ambient * lightSource.lightModelAmbient), 1.0);
    for(int i = 0; i < 3; i++) {
        spotCalcul = computeSpot(attribIn.spotDir[i], attribIn.lightDir[i], attribIn.normal);
        reflexion = computeLight(i, attribIn.normal, attribIn.lightDir[i], attribIn.obsPos);
        coul[i] = vec4(reflexion * spotCalcul, 1.0);
    }

    Finalcoul += coul[0] + coul[1] + coul[2];
    FragColor = clamp(Finalcoul, 0.0, 1.0);
}
