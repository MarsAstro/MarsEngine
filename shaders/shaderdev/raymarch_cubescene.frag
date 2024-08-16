#version 330 core
out vec4 FragmentColor;

in vec2 TextureCoordinates;
vec2 uvs = TextureCoordinates;

uniform vec2 resolution;
uniform sampler2D diffuse1;
uniform float time;

const int NUM_STEPS = 256;
const float MAX_DIST = 1000.0;

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

struct MaterialData
{
    vec3 color;
    float dist;
};

MaterialData map(vec3 pos)
{
    MaterialData result = MaterialData(GRAY, sdPlane(pos - vec3(0.0, -2.0, 0.0)));

    float dist = sdRoundBox(pos - vec3(-2.0, -0.85, 5.0), vec3(1.0), 0.2);
    result.color = dist < result.dist ? RED : result.color;
    result.dist = min(dist, result.dist);

    dist = sdRoundBox(pos - vec3(2.0, -0.85, 5.0), vec3(1.0), 0.2);
    result.color = dist < result.dist ? BLUE : result.color;
    result.dist = min(dist, result.dist);

    dist = sdRoundBox(pos - vec3(2.0, 1.0, 50.0 + sin(time) * 25.0), vec3(2.0), 0.6);
    result.color = dist < result.dist ? GREEN : result.color;
    result.dist = min(dist, result.dist);

    return result;
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
    float d = 0.01;

    for (int i = 0; i < 64; ++i)
    {
        float distToScene = map(pos + lightDir * d).dist;

        if (distToScene < 0.001)
        return 0.05;

        d += distToScene;
    }

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
    vec3 pos;
    vec3 skyColor = mix(vec3(0.55, 0.6, 1.0), vec3(0.1, 0.1, 1.0), cameraDir.y);
    MaterialData material = MaterialData(vec3(0.0), 0.0);

    for (int i = 0; i < NUM_STEPS; ++i)
    {
        pos = cameraOrigin + material.dist * cameraDir;

        MaterialData result = map(pos);

        if (result.dist < 0.001)
        {
            break;
        }
        material.dist += result.dist;
        material.color = result.color;

        if (material.dist > MAX_DIST)
        {
            return skyColor;
        }
    }

    vec3 lightColor = WHITE;
    vec3 lightDir = normalize(vec3(1.0, 2.0, -1.0));
    vec3 normal = CalculateNormal(pos);
    float shadowed = CalculateShadow(pos, lightDir);
    vec3 lighting = CalculateLighting(pos, normal, lightColor, lightDir);
    lighting *= shadowed;

    vec3 color = material.color * lighting;
    float fogFactor = 1.0 - exp(-pos.z * 0.0015);
    color = mix(color, skyColor, fogFactor);

    return color;
}

void main()
{
    vec2 pixelCoords = (uvs - 0.5) * resolution;

    vec3 rayDir = normalize(vec3(pixelCoords * 2.0 / resolution.y, 1.0));
    vec3 rayOrigin = vec3(0.0);

    vec3 color = RayMarch(rayOrigin, rayDir);

    FragmentColor = vec4(color, 1.0);
}