#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normal;
out ATTRIB_VS_OUT {
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

struct Material {
    vec3 emission;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

Light computeLight(
    in int lightIndex,
    in vec3 normal,
    in vec3 lightDir,
    in vec3 obsPos
) {
    vec3 N = normalize(normal); // vecteur normal
    vec3 L = normalize(lightDir); // vecteur vers la lumière
    vec3 O = normalize(obsPos); // position de l'observateur
    Light light;
    light.ambient = vec3(0);
    light.diffuse = vec3(0);
    light.specular = vec3(0);
    float attenuation = 1.0;

    float d = length(L);

    // calculer la composante ambiante pour la source de lumière
    light.ambient += vec3((lightSource.mat.ambient * lightSource.lights[lightIndex].ambient));

    float NdotL = max(0.0, dot(N, L));
    if(NdotL > 0.0) {
        // calculer la composante diffuse
        light.diffuse += attenuation * lightSource.mat.diffuse * lightSource.lights[lightIndex].diffuse * NdotL;

        // calculer la composante spéculaire (Blinn ou Phong : spec = BdotN ou RdotO )
        float spec = (lightSource.useBlinn ? dot(normalize(L + O), N) : // dot( B, N )
        dot(reflect(-L, N), O)); // dot( R, O )
        if(spec > 0) {
            light.specular += attenuation * lightSource.mat.specular * lightSource.lights[lightIndex].specular * pow(spec, lightSource.mat.shininess);
        }
    }
    return light;
}

void main() {
    float spotCalcul[3];
    Light light[3];
    vec3 lightDir[3];
    vec3 spotDir[3];

    gl_Position = mvp * vec4(position, 1.0);
    attribOut.texCoords = texCoords;
    // calculer la normale (N)
    vec3 CalculNormal = normalMatrix * normal;

    // calculer la position du sommet dans le repère de la caméra
    vec3 pos = (modelView * vec4(position, 1.0)).xyz;
     // Vecteur O
    vec3 obsPos = (-pos);
    for(int i = 0; i < 3; i++) {
        lightDir[i] = (view * vec4((lightSource.lights[i].position), 1.0)).xyz - pos;
        spotDir[i] = mat3(view) * -lightSource.lights[i].spotDirection;
    }

    for(int i = 0; i < 3; i++) {
        spotCalcul[i] = computeSpot(spotDir[i], lightDir[i], CalculNormal);
        light[i] = computeLight(i, CalculNormal, lightDir[i], obsPos);
    }
    attribOut.emission =  lightSource.mat.emission + lightSource.mat.ambient * lightSource.lightModelAmbient;
    attribOut.ambient = light[0].ambient * spotCalcul[0] + light[1].ambient * spotCalcul[1] + light[2].ambient * spotCalcul[2];
    attribOut.diffuse = light[0].diffuse * spotCalcul[0] + light[1].diffuse * spotCalcul[1] + light[2].diffuse * spotCalcul[2];
    attribOut.specular = light[0].specular * spotCalcul[0] + light[1].specular * spotCalcul[1] + light[2].specular * spotCalcul[2];

}
