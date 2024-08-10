#version 330 core
out vec4 FragmentColor;

in vec2 TextureCoordinates;
vec2 uvs = TextureCoordinates;

uniform vec2 resolution;
uniform sampler2D diffuse1;
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

void main()
{
    vec2 coords = fract(uvs * vec2(2.0, 1.0));
    coords.x = remap(coords.x, 0.0, 1.0, 0.25, 0.75);

    vec3 color = vec3(texture(diffuse1, coords));

    if (uvs.x > 0.5)
    {
        // Tinting
        vec3 tintColor = vec3(1.0, 0.5, 0.5);
        //color *= tintColor;

        // Brightness
        float brightnessAmount = 0.1;
        //color += brightnessAmount;

        // Contrast
        float contrastAmount = 1.0;
        float midpoint = 0.5;
        //color = (color - midpoint) * contrastAmount + midpoint;

        // Saturation
        float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
        float saturationAmount = 1.0;
        //color = mix(vec3(luminance), color, saturationAmount);

        // The Matrix
        //color = pow(color, vec3(1.5, 0.8, 1.5));

        // Pixelation
        vec2 dims = vec2(128.0, 128.0);
        vec2 texUV = floor(coords * dims) / dims;
        vec3 pixelated = texture(diffuse1, texUV).xyz;
        //color = pixelated;

        // Distortion
        vec3 sgn = sign(color - 0.5);
        vec2 pushedCoords = coords;
        float pushedSign = sign(pushedCoords.y - 0.5);
        pushedCoords.y = pushedSign * pow(abs(pushedCoords.y - 0.5) * 2.0, 0.8) * 0.5 + 0.5;
        //color = texture(diffuse1, pushedCoords).xyz;

        // Ripples
        float distToCenter = length(coords - 0.5);
        float d = sin(distToCenter * 50.0 - time * 2.0);
        vec2 dir = normalize(coords - 0.5);
        vec2 rippleCoords = coords + d * dir * 0.05;
        color = texture(diffuse1, rippleCoords).xyz;
    }

    FragmentColor = vec4(color, 1.0);
}