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

float random(vec2 p)  // replace this by something better
{
    p  = 50.0*fract( p*0.3183099 + vec2(0.71,0.113));
    return -1.0+2.0*fract( p.x*p.y*(p.x+p.y) );
}

float noise(vec2 coords) {
    vec2 texSize = vec2(1.0);
    vec2 pc = coords * texSize;
    vec2 base = floor(pc);

    float s1 = random((base + vec2(0.0, 0.0)) / texSize);
    float s2 = random((base + vec2(1.0, 0.0)) / texSize);
    float s3 = random((base + vec2(0.0, 1.0)) / texSize);
    float s4 = random((base + vec2(1.0, 1.0)) / texSize);

    vec2 f = smoothstep(0.0, 1.0, fract(pc));

    float px1 = mix(s1, s2, f.x);
    float px2 = mix(s3, s4, f.x);
    float result = mix(px1, px2, f.y);
    return result;
}

float fbm(vec2 p, int octaves, float persistence, float lacunarity) {
    float amplitude = 0.5;
    float total = 0.0;

    for (int i = 0; i < octaves; ++i) {
        float noiseValue = noise(p);
        total += noiseValue * amplitude;
        amplitude *= persistence;
        p = p * lacunarity;
    }

    return total;
}

float sdSphere(vec3 p, float r)
{
    return length(p) - r;
}

float sdRoundBox(vec3 p, vec3 b, float r)
{
    vec3 q = abs(p) - b + r;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - r;
}

float sdBox(vec3 p, vec3 b)
{
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sdTorus(vec3 p, vec2 t)
{
    vec2 q = vec2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

float sdPlane(vec3 pos)
{
    return pos.y;
}

vec3 RED = vec3(1.0, 0.0, 0.0);
vec3 GREEN = vec3(0.0, 1.0, 0.0);
vec3 BLUE = vec3(0.0, 0.0, 1.0);
vec3 GRAY = vec3(0.5);
vec3 WHITE = vec3(1.0);

const int NUM_STEPS = 256;
const float MAX_DIST = 1000.0;
const float MIN_DIST = 0.00001;

struct MaterialData
{
    vec3 color;
    float dist;
};

MaterialData materialMin(MaterialData matA, MaterialData matB)
{
    if (matA.dist < matB.dist)
        return matA;

    return matB;
}

MaterialData map(vec3 pos)
{
    float curNoiseSample = fbm(pos.xz / 2.0, 1, 0.5, 2.0);
    curNoiseSample = abs(curNoiseSample);
    curNoiseSample *= 1.5;
    curNoiseSample += 0.1 * fbm(pos.xz * 4.0, 8, 0.5, 2.0);

    float waterLevel = 0.45;
    float mountainLevel = 0.3;
    float snowLevel = 0.1;
    vec3 landColor = vec3(0.498, 0.435, 0.396);
    landColor = mix(landColor, landColor * 0.25, smoothstep(waterLevel - 0.1, waterLevel, curNoiseSample));
    landColor = mix(landColor, vec3(0.5), smoothstep(mountainLevel, mountainLevel - 0.05, curNoiseSample));
    landColor = mix(landColor, WHITE, smoothstep(snowLevel, snowLevel - 0.1, curNoiseSample));
    MaterialData result = MaterialData(landColor, pos.y + curNoiseSample);

    vec3 shallowColor = vec3(0.25, 0.25, 0.75);
    vec3 deepColor = vec3(0.025, 0.025, 0.15);
    vec3 waterColor = mix(shallowColor, deepColor, smoothstep(waterLevel, waterLevel + 0.1, curNoiseSample));
    waterColor = mix(waterColor, WHITE, smoothstep(waterLevel + 0.0125, waterLevel, curNoiseSample));
    MaterialData waterMaterial = MaterialData(waterColor, pos.y + waterLevel);

    result = materialMin(result, waterMaterial);

    return result;
}

MaterialData RayCast(vec3 cameraOrigin, vec3 cameraDir, int numSteps, float startDist, float maxDist)
{
    vec3 pos;
    MaterialData material = MaterialData(vec3(0.0), startDist);
    MaterialData defaultMaterial = MaterialData(vec3(0.0), -1.0);

    for (int i = 0; i < numSteps; ++i)
    {
        pos = cameraOrigin + material.dist * cameraDir;

        MaterialData result = map(pos);

        if (abs(result.dist) < MIN_DIST * material.dist)
        {
            break;
        }
        material.dist += result.dist;
        material.color = result.color;

        if (material.dist > maxDist)
        {
            return defaultMaterial;
        }
    }

    return material;
}

vec3 CalculateNormal(vec3 pos)
{
    const float EPS = 0.0001;
    vec3 n = vec3(
        map(pos + vec3(EPS, 0.0, 0.0)).dist - map(pos - vec3(EPS, 0.0, 0.0)).dist,
        map(pos + vec3(0.0, EPS, 0.0)).dist - map(pos - vec3(0.0, EPS, 0.0)).dist,
        map(pos + vec3(0.0, 0.0, EPS)).dist - map(pos - vec3(0.0, 0.0, EPS)).dist
    );

    return normalize(n);
}

vec3 CalculateLighting(vec3 pos, vec3 normal, vec3 lightColor, vec3 lightDir)
{
    float dp = saturate(dot(normal, lightDir));

    return lightColor * dp;
}

float CalculateShadow(vec3 pos, vec3 lightDir)
{
    MaterialData result = RayCast(pos, lightDir, 64, 0.01, 10.0);

    if(result.dist >= 0.0)
        return 0.1;

    return 1.0;
}

float CalculateAO(vec3 pos, vec3 normal)
{
    float ao = 0.0;
    float stepSize = 0.1;

    for (float i = 0.0; i < 5.0; i++)
    {
        float distFactor = 1.0 / pow(2.0, i);

        ao += distFactor * (i * stepSize - map(pos + normal * i * stepSize).dist);
    }

    return 1.0 - ao;
}

vec3 RayMarch(vec3 cameraOrigin, vec3 cameraDir)
{
    MaterialData material = RayCast(cameraOrigin, cameraDir, NUM_STEPS, 1.0, MAX_DIST);
    vec3 lightColor = WHITE;
    vec3 lightDir = normalize(vec3(1.5, 0.5, -0.0));
    float skyT = exp(saturate(cameraDir.y) * -6.0);
    float sunFactor = pow(saturate(dot(lightDir, cameraDir)), 8.0);
    vec3 skyColor = mix(vec3(0.025, 0.065, 0.5), vec3(0.4, 0.5, 1.0), skyT);
    vec3 fogColor = mix(skyColor, vec3(1.0, 0.9, 0.65), sunFactor);

    if (material.dist < 0.0)
        return fogColor;

    vec3 pos = cameraOrigin + material.dist * cameraDir;
    vec3 normal = CalculateNormal(pos);
    float shadowed = CalculateShadow(pos, lightDir);
    vec3 lighting = CalculateLighting(pos, normal, lightColor, lightDir);
    lighting *= shadowed;

    float fogDist = distance(cameraOrigin, pos);
    float inscatter = 1.0 - exp(-fogDist * fogDist * mix(0.0008, 0.001, sunFactor));
    float extinction = exp(-fogDist * fogDist * 0.002);

    vec3 color = material.color * lighting;
    color = color * extinction + fogColor * inscatter;

    return color;
}

mat3 makeCameraMatrix(vec3 cameraOrigin, vec3 cameraLookAt, vec3 cameraUp)
{
    vec3 z = normalize(cameraLookAt - cameraOrigin);
    vec3 x = normalize(cross(z, cameraUp));
    vec3 y = cross(x, z);

    return mat3(x, y, z);
}

void main()
{
    vec2 pixelCoords = (uvs - 0.5) * resolution;

    float t = time * 0.25;
    vec3 rayDir = normalize(vec3(pixelCoords * 2.0 / resolution.y, 1.0));
    vec3 rayOrigin = vec3(3.0, remap(sin(time) * 0.05, -1.0, 1.0, 0.2, 2.0), -3.0) * vec3(cos(t), 1.0, sin(t)) ;
    vec3 rayLookAt = vec3(0.0);
    mat3 camera = makeCameraMatrix(rayOrigin, rayLookAt, vec3(0.0, 1.0, 0.0));

    vec3 color = RayMarch(rayOrigin, camera * rayDir);

    FragmentColor = vec4(color, 1.0);
}