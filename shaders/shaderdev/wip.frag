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

    return mix(color1, color2, smoothstep(0.0, 1.0, uvs.y));
}

vec3 DrawMountains(vec3 background, vec3 mountainColor, vec2 pixelCoords)
{
    float y = sin(pixelCoords.x / 256.0) * 256.0;

    float sdfMountain = pixelCoords.y - y;

    vec3 color = mix(mountainColor, background, smoothstep(0.0, 1.0, sdfMountain));

    return color;
}


void main()
{
    vec2 pixelCoords = (uvs - 0.5) * resolution;
    vec3 color = GenerateSky();

    color = DrawMountains(color, vec3(0.0), pixelCoords);

    FragmentColor = vec4(color, 1.0);
}