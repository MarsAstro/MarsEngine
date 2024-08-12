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

float saturate(float x)
{
    return clamp(x, 0.0, 1.0);
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

float fbm(vec3 p, int octaves, float persistence, float lacunarity, float exponentiation)
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

    total = pow(total, exponentiation);

    return total;
}

vec3 GenerateGridStars(vec2 pixelCoords, float starRadius, float cellWidth, float seed, bool twinkle)
{
    vec2 cellCoords = (fract(pixelCoords / cellWidth) - 0.5) * cellWidth;
    vec2 cellID = floor(pixelCoords / cellWidth) + seed / 100.0;
    vec3 cellHashValue = hash(vec3(cellID, 0.0));

    float starBrightness = saturate(cellHashValue.z);
    vec2 starPosition = vec2(0.0);
    starPosition += cellHashValue.xy * (cellWidth * 0.5 - starRadius * 4.0);
    float distToStar = length(cellCoords - starPosition);

    float glow = exp(-2.0 * distToStar / starRadius);

    if (twinkle) {
        float noiseSample = noise(vec3(cellID, time * 1.5));
        float twinkleSize = remap(noiseSample, -1.0, 1.0, 1.0, 0.1) * starRadius * 6.0;
        vec2 absDist = abs(cellCoords - starPosition);
        float twinkleValue = smoothstep(starRadius * 0.25, 0.0, absDist.y)
            * smoothstep(twinkleSize, 0.0, absDist.x);
        twinkleValue += smoothstep(starRadius * 0.25, 0.0, absDist.x)
            * smoothstep(twinkleSize, 0.0, absDist.y);
        glow += twinkleValue;
    }

    return vec3(glow * starBrightness);
}

vec3 GenerateStars(vec2 pixelCoords)
{
    vec3 stars = vec3(0.0);

    float size = 4.0;
    float cellWidth = 500.0;
    for (float i = 0.0; i <= 2.0; i++)
    {
        stars += GenerateGridStars(pixelCoords, size, cellWidth, i, true);
        size *= 0.5;
        cellWidth *= 0.35;
    }

    for (float i = 3.0; i < 5.0; i++)
    {
        stars += GenerateGridStars(pixelCoords, size, cellWidth, i, false);
        size *= 0.5;
        cellWidth *= 0.35;
    }

    return stars;
}

float sdfCircle(vec2 p, float r)
{
    return length(p) - r;
}

float map(vec3 pos)
{
    return fbm(pos, 6, 0.5, 2.0, 4.0);
}

vec3 calcNormal(vec3 pos, vec3 n)
{
    vec2 e = vec2(0.0001, 0.0);
    return normalize(
        n + -500.0 * vec3(
            map(pos + e.xyy) - map(pos - e.xyy),
            map(pos + e.yxy) - map(pos - e.yxy),
            map(pos + e.yyx) - map(pos - e.yyx)
        )
    );
}

mat3 rotateY(float radians)
{
    float sn = sin(radians);
    float cs = cos(radians);
    return mat3(
        cs, 0.0, sn,
        0.0, 1.0, 0.0,
        -sn, 0.0, cs
    );
}

vec3 DrawPlanet(vec2 pixelCoords, vec3 color)
{
    float planetRadius = 400.0;
    float d = sdfCircle(pixelCoords, planetRadius);

    vec3 planetColor = vec3(1.0);

    if (d <= 0.0)
    {
        float x = pixelCoords.x / planetRadius;
        float y = pixelCoords.y / planetRadius;
        float z = sqrt(1.0 - x * x - y * y);
        mat3 planetRotation = rotateY(time * 0.1);
        vec3 viewNormal = vec3(x, y, z);
        vec3 wsPosition = planetRotation * viewNormal;
        vec3 wsNormal = planetRotation * normalize(wsPosition);
        vec3 wsViewDir = planetRotation * vec3(0.0, 0.0, 1.0);

        vec3 noiseCoord = wsPosition * 2.1;
        float noiseSample = fbm(noiseCoord, 6, 0.5, 2.2, 4.5);
        float moistureMap = fbm(noiseCoord * 0.5 + vec3(20.0), 6, 0.5, 2.0, 1.0);

        // Coloring
        vec3 waterColor = mix(vec3(0.01, 0.09, 0.55), vec3(0.09, 0.26, 0.57), smoothstep(0.02, 0.06, noiseSample));
        vec3 landColor = mix(vec3(0.5, 1.0, 0.3), vec3(0.0, 0.7, 0.0), smoothstep(0.05, 0.1, noiseSample));
        landColor = mix(vec3(1.0, 1.0, 0.5), landColor, smoothstep(0.4, 0.5, moistureMap));
        landColor = mix(landColor, vec3(0.5), smoothstep(0.10, 0.15, noiseSample));
        landColor = mix(landColor, vec3(1.0), smoothstep(0.15, 0.2, noiseSample));
        landColor = mix(landColor, vec3(0.9), smoothstep(0.6, 0.9, abs(viewNormal.y)));

        planetColor = mix(waterColor, landColor, smoothstep(0.05, 0.06, noiseSample));

        // Lighting
        vec2 specParams = mix(vec2(0.5, 32.0), vec2(0.01, 2.0), smoothstep(0.05, 0.06, noiseSample));
        vec3 wsLightDir = planetRotation * normalize(vec3(0.5, 1.0, 0.5));
        vec3 wsSurfaceNormal = calcNormal(noiseCoord, wsNormal);
        float wrap = 0.05;
        float dp = max(0.0, (dot(wsLightDir, wsSurfaceNormal) + wrap) / (1.0 + wrap));

        vec3 lightColor = mix(vec3(0.25, 0.0, 0.0), vec3(0.75), smoothstep(0.05, 0.5, dp));
        vec3 ambient = vec3(0.002);
        vec3 diffuse = lightColor * dp;

        vec3 r = normalize(reflect(-wsLightDir, wsSurfaceNormal));
        float phongValue = max(0.0, dot(wsViewDir, r));
        phongValue = pow(phongValue, specParams.y);

        vec3 specular = vec3(phongValue) * 0.5 * diffuse;

        vec3 planetShading = planetColor * (diffuse + ambient) + specular;
        planetColor = planetShading;

        // Fresnel
        float fresnel = smoothstep(1.0, 0.1, viewNormal.z);
        fresnel = pow(fresnel, 8.0) * dp;
        planetColor = mix(planetColor, vec3(0.0, 0.5, 1.0), fresnel);
    }

    color = mix(color, planetColor, smoothstep(0.0, -1.0, d));

    float glowRadius = 40.0;
    if (d < glowRadius && d >= -1.0)
    {
        float radius = planetRadius + glowRadius;
        float x = pixelCoords.x / radius;
        float y = pixelCoords.y / radius;
        float z = sqrt(1.0 - x * x - y * y);
        vec3 normal = vec3(x, y, z);

        float lighting = dot(normal, normalize(vec3(0.5, 1.0, 0.5)));
        lighting = smoothstep(-0.15, 1.0, lighting);

        vec3 glowColor = vec3(0.05, 0.3, 0.9) * exp(-0.01 * d * d) * lighting * 0.75;
        color += glowColor;
    }

    return color;
}

void main()
{
    vec2 pixelCoords = (uvs - 0.5) * resolution;

    vec3 color = vec3(0.0);
    color = GenerateStars(pixelCoords);
    color = DrawPlanet(pixelCoords, color);

    FragmentColor = vec4(color, 1.0);
}