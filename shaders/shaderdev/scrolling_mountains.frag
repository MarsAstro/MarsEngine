#version 330 core
out vec4 FragmentColor;

in vec2 TextureCoordinates;
vec2 uvs = TextureCoordinates;

uniform vec2 resolution;
uniform float time;

float inverseLerp(float v, float minValue, float maxValue)
{
    return (v - minValue) / (maxValue - minValue);
}

float remap(float v, float inMin, float inMax, float outMin, float outMax)
{
    float t = inverseLerp(v, inMin, inMax);
    return mix(outMin, outMax, t);
}

vec3 hash(vec3 p)
{
    p = vec3(dot(p, vec3(127.1, 311.7,  74.7)),
             dot(p, vec3(269.5, 183.3, 246.1)),
             dot(p, vec3(113.5, 271.9, 124.6)));

    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(vec3 p)
{
    vec3 i = floor(p);
    vec3 f = fract(p);

    vec3 u = f * f * (3.0 - 2.0 * f);

    return mix(mix(mix(dot(hash(i + vec3(0.0,0.0,0.0)), f - vec3(0.0,0.0,0.0)),
                       dot(hash(i + vec3(1.0,0.0,0.0)), f - vec3(1.0,0.0,0.0)), u.x),
                   mix(dot(hash(i + vec3(0.0,1.0,0.0)), f - vec3(0.0,1.0,0.0)),
                       dot(hash(i + vec3(1.0,1.0,0.0)), f - vec3(1.0,1.0,0.0)), u.x), u.y),
               mix(mix(dot(hash(i + vec3(0.0,0.0,1.0)), f - vec3(0.0,0.0,1.0)),
                       dot(hash(i + vec3(1.0,0.0,1.0)), f - vec3(1.0,0.0,1.0)), u.x),
                   mix(dot(hash(i + vec3(0.0,1.0,1.0)), f - vec3(0.0,1.0,1.0)),
                       dot(hash(i + vec3(1.0,1.0,1.0)), f - vec3(1.0,1.0,1.0)), u.x), u.y), u.z);
}

float fbm(vec3 p, int octaves, float persistence, float lacunarity)
{
    float amplitude = 0.5;
    float frequency = 1.0;
    float total = 0.0;
    float normalization = 0.0;

    for (int i = 0; i < octaves; ++i)
    {
        float noiseValue = noise(p * frequency);
        total += noiseValue * amplitude;
        normalization += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    total /= normalization;
    total = smoothstep(-1.0, 1.0, total);

    return total;
}

vec3 GenerateSky()
{
    vec3 color1 = vec3(0.4, 0.6, 0.9);
    vec3 color2 = vec3(0.1, 0.15, 0.4);

    return mix(color1, color2, smoothstep(0.875, 1.0, uvs.y));
}

vec3 DrawMountains(vec3 background, vec3 mountainColor, vec2 pixelCoords, float depth)
{
    float y = fbm(vec3(depth + pixelCoords.x / 256.0, 1.432, 3.643), 6, 0.5, 2.0) * 256.0;

    vec3 fogColor = vec3(0.4, 0.6, 0.9);
    float fogFactor = smoothstep(0.0, 8000.0, depth) * 0.5;

    float heightFactor = smoothstep(64.0, -128.0, pixelCoords.y) * 0.5;
    heightFactor *= heightFactor;
    fogFactor = mix(heightFactor, fogFactor, fogFactor);

    mountainColor = mix(mountainColor, fogColor, fogFactor);

    float sdfMountain = pixelCoords.y - y;

    float blur = 1.0 + smoothstep(200.0, 6000.0, depth) * 128.0
        + smoothstep(200.0, -1400.0, depth) * 128.0;
    vec3 color = mix(mountainColor, background, smoothstep(0.0, blur, sdfMountain));

    return color;
}

void main()
{
    vec2 pixelCoords = (uvs - 0.5) * resolution;
    vec3 color = GenerateSky();

    vec2 timeOffset = vec2(time * 50.0, 0.0);
    vec2 mountainCoords = (pixelCoords - vec2(0.0, 400.0)) * 8.0 + timeOffset;
    color = DrawMountains(color, vec3(0.5), mountainCoords, 6000.0);

    mountainCoords = (pixelCoords - vec2(0.0, 360.0)) * 4.0 + timeOffset;
    color = DrawMountains(color, vec3(0.45), mountainCoords, 3200.0);

    mountainCoords = (pixelCoords - vec2(0.0, 280.0)) * 2.0 + timeOffset;
    color = DrawMountains(color, vec3(0.40), mountainCoords, 1600.0);

    mountainCoords = (pixelCoords - vec2(0.0, 150.0)) * 1.0 + timeOffset;
    color = DrawMountains(color, vec3(0.35), mountainCoords, 800.0);

    mountainCoords = (pixelCoords - vec2(0.0, -100.0)) * 0.5 + timeOffset;
    color = DrawMountains(color, vec3(0.30), mountainCoords, 400.0);

    mountainCoords = (pixelCoords - vec2(0.0, -500.0)) * 0.25 + timeOffset;
    color = DrawMountains(color, vec3(0.25), mountainCoords, 200.0);

    mountainCoords = (pixelCoords - vec2(0.0, -1400.0)) * 0.125 + timeOffset;
    color = DrawMountains(color, vec3(0.20), mountainCoords, 0.0);

    FragmentColor = vec4(color, 1.0);
}