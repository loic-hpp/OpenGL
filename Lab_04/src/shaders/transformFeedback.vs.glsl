#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 velocity;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 size;
layout(location = 4) in float timeToLive;

out vec3 positionMod;
out vec3 velocityMod;
out vec4 colorMod;
out vec2 sizeMod;
out float timeToLiveMod;

uniform float time;
uniform float dt;

uint seed = uint(time * 1000.0) + uint(gl_VertexID);
uint randhash() // entre  0 et UINT_MAX
{
    uint i = ((seed++) ^ 12345391u) * 2654435769u;
    i ^= (i << 6u) ^ (i >> 26u);
    i *= 2654435769u;
    i += (i << 5u) ^ (i >> 12u);
    return i;
}
float random() // entre  0 et 1
{
    const float UINT_MAX = 4294967295.0;
    return float(randhash()) / UINT_MAX;
}

const float PI = 3.14159265359;
vec3 randomInCircle(in float radius, in float height) {
    float r = radius * sqrt(random());
    float theta = random() * 2 * PI;
    return vec3(r * cos(theta), height, r * sin(theta));
}

const float MAX_TIME_TO_LIVE = 2.0;
const float INITIAL_RADIUS = 0.5;
const float INITIAL_X = 0.1;
const float INITIAL_HEIGHT = 0.0;
const float FINAL_RADIUS = 0.5;
const float FINAL_HEIGHT = 5.0;

const float INITIAL_SPEED_MIN = 0.5;
const float INITIAL_SPEED_MAX = 0.6;

const float INITIAL_TIME_TO_LIVE_RATIO = 0.85;

const float INITIAL_ALPHA = 0.0;
const float ALPHA = 0.1;
const vec3 YELLOW_COLOR = vec3(1.0, 0.9, 0.0);
const vec3 ORANGE_COLOR = vec3(1.0, 0.4, 0.2);
const vec3 DARK_RED_COLOR = vec3(0.1, 0.0, 0.0);

const vec3 ACCELERATION = vec3(0.0, 0.1, 0.0);

void main() {
    // TODO   
    // créer une nouvelle particule si celle-ci est en fin de vie
    if(timeToLive <= 0.0) {
        vec3 initialPosition = randomInCircle(INITIAL_X, INITIAL_HEIGHT);
        // on normalise car il s'agit juste de la direction
        vec3 direction = normalize(randomInCircle(FINAL_RADIUS, FINAL_HEIGHT));
        float initialSpeed = mix(INITIAL_SPEED_MAX, INITIAL_SPEED_MIN, random());
        vec3 initialVelocity = direction * initialSpeed;

        positionMod = initialPosition;
        velocityMod = initialVelocity; 
        colorMod = vec4(YELLOW_COLOR, INITIAL_ALPHA);
        sizeMod = vec2(INITIAL_RADIUS / 2, INITIAL_RADIUS);

        timeToLiveMod = mix(MAX_TIME_TO_LIVE * INITIAL_TIME_TO_LIVE_RATIO, MAX_TIME_TO_LIVE, random());
    } else {
        // Mettre à jour la particule

        positionMod = position + velocity * dt;
        velocityMod = velocity + ACCELERATION * dt;
        timeToLiveMod = timeToLive - dt;
        float normalizedTime = 1.0 - (timeToLiveMod / MAX_TIME_TO_LIVE);
        float sizeFactor = mix(1.0, 1.5, smoothstep(0.5, 1.0, normalizedTime));
        float updatedAlpha = smoothstep(0.0, 0.2, normalizedTime) * ALPHA + (1.0 - smoothstep(0.8, 1.0, normalizedTime)) * ALPHA;
        sizeMod = size;


        if(normalizedTime >= 0.0 && normalizedTime < 0.25) {
            colorMod = vec4(YELLOW_COLOR, updatedAlpha);
        } else if(normalizedTime >= 0.25 && normalizedTime < 0.3) {
            colorMod = vec4(mix(YELLOW_COLOR, ORANGE_COLOR, smoothstep(0.25, 0.3, normalizedTime)), updatedAlpha);
        } else if(normalizedTime >= 0.3 && normalizedTime < 0.5) {
            colorMod = vec4(ORANGE_COLOR, updatedAlpha);
        } else {
            colorMod = vec4(mix(ORANGE_COLOR, DARK_RED_COLOR, smoothstep(0.5, 1.0, normalizedTime)), updatedAlpha);
            sizeMod = vec2(INITIAL_RADIUS * sizeFactor, INITIAL_RADIUS);
        }

    }

}