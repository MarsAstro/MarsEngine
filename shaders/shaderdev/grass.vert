#version 330 core
layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};
uniform mat4 model;
uniform float time;
uniform vec4 grassParams;
uniform vec3 cameraPosition;

out vec3 VertexColor;
out vec3 VertexNormal;
out vec3 FragmentPosition;
out vec4 GrassData;

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

uvec2 murmurHash21(uint src) {
    const uint M = 0x5bd1e995u;
    uvec2 h = uvec2(1190494759u, 2147483647u);
    src *= M;
    src ^= src>>24u;
    src *= M;
    h *= M;
    h ^= src;
    h ^= h>>13u;
    h *= M;
    h ^= h>>15u;
    return h;
}

// 2 outputs, 1 input
vec2 hash21(float src) {
    uvec2 h = murmurHash21(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

float easeOut(float x, float t)
{
    return 1.0 - pow(1.0 - x, t);
}

mat3 rotateY(float theta)
{
    float sn = sin(theta);
    float cs = cos(theta);

    return mat3
    (
        cs,  0.0, sn,
        0.0, 1.0, 0.0,
        -sn, 0.0, cs
    );
}

mat3 rotateAxis(vec3 axis, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat3(
        oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
        oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
        oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c
    );
}

vec3 bezier(vec3 P0, vec3 P1, vec3 P2, vec3 P3, float t) {
    return (1.0 - t) * (1.0 - t) * (1.0 - t) * P0 +
        3.0 * (1.0 - t) * (1.0 - t) * t * P1 +
        3.0 * (1.0 - t) * t * t * P2 +
        t * t * t * P3;
}

vec3 bezierGrad(vec3 P0, vec3 P1, vec3 P2, vec3 P3, float t) {
    return 3.0 * (1.0 - t) * (1.0 - t) * (P1 - P0) +
        6.0 * (1.0 - t) * t * (P2 - P1) +
        3.0 * t * t * (P3 - P2);
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

void main()
{
    const float PI = 3.14159;
    const vec3 BASE_COLOR = vec3(0.1, 0.4, 0.04);
    const vec3 TIP_COLOR = vec3(0.5, 0.7, 0.3);
    int GRASS_SEGMENTS = int(grassParams.x);
    int GRASS_VERTICES = (GRASS_SEGMENTS + 1) * 2;
    float GRASS_PATCH_SIZE = grassParams.y;
    float GRASS_WIDTH = grassParams.z;
    float GRASS_HEIGHT = grassParams.w;

    // Figure out grass offset
    vec2 hashedInstanceID = hash21(float(gl_InstanceID)) * 2.0 - 1.0;
    vec3 grassOffset = vec3(hashedInstanceID.x, 0.0, hashedInstanceID.y) * GRASS_PATCH_SIZE;

    vec3 grassBladeWorldPos = (model * vec4(grassOffset, 1.0)).xyz;
    vec3 posHash = hash(grassBladeWorldPos);
    float angle = remap(posHash.x, -1.0, 1.0, -PI, PI);

    // Find out vertex id, > GRASS_VERTICES is other side
    int vertFB_ID = gl_VertexID % (GRASS_VERTICES * 2);
    int vertID = vertFB_ID % GRASS_VERTICES;

    // 0 - left, 1 = right
    int xTest = vertID & 0x1;
    int zTest = (vertFB_ID >= GRASS_VERTICES) ? 1 : -1;
    float xSide = float(xTest);
    float zSide = float(zTest);
    float heightPercent = float(vertID - xTest) / (float(GRASS_SEGMENTS) * 2.0);

    float width = GRASS_WIDTH * easeOut(1.0 - heightPercent, 4.0);
    float height = GRASS_HEIGHT;

    // Calculate the vertex position;
    float x = (xSide - 0.5) * width;
    float y = heightPercent * height;
    float z = 0.0;

    // Grass lean factor
    float windStrength = noise(vec3(grassBladeWorldPos.xz * 0.05, 0.0) + time);
    float windAngle = 0.0;
    vec3 windAxis = vec3(cos(windAngle), 0.0, sin(windAngle));
    float windLeanAngle = windStrength * 1.5 * heightPercent;
    float randomLeanAnimation = noise(vec3(grassBladeWorldPos.xz, time * 4.0)) * (windStrength * 0.5 + 0.125);
    float leanFactor = remap(posHash.y, -1.0, 1.0, 0.0, 0.5) + randomLeanAnimation;

    // Add bezier curve for bend
    vec3 p1 = vec3(0.0);
    vec3 p2 = vec3(0.0, 0.33, 0.0);
    vec3 p3 = vec3(0.0, 0.66, 0.0);
    vec3 p4 = vec3(0.0, cos(leanFactor), sin(leanFactor));
    vec3 curve = bezier(p1, p2, p3, p4, heightPercent);

    // Caluclate normal
    vec3 curveGrad = bezierGrad(p1, p2, p3, p4, heightPercent);
    mat2 curveRot90 = mat2(0.0, 1.0, -1.0, 0.0) * -zSide;

    // Apply curve
    y = curve.y * height;
    z = curve.z * height;

    // Generate grass vertex data
    mat3 grassMat = rotateAxis(windAxis, windLeanAngle) * rotateY(angle);
    vec3 grassLocalPosition = grassMat * vec3(x, y, z) + grassOffset;
    vec3 grassLocalNormal = grassMat * vec3(0.0, curveRot90 * curveGrad.yz);

    // Grass color
    vec3 c1 = mix(BASE_COLOR, TIP_COLOR, heightPercent);
    vec3 c2 = mix(vec3(0.6, 0.6, 0.4), vec3(0.88, 0.87, 0.52), heightPercent);
    float noiseValue = noise(grassBladeWorldPos * 0.1);

    // Blend normal
    float distanceBlend = smoothstep(0.0, 10.0, distance(cameraPosition, grassBladeWorldPos));
    grassLocalNormal = mix(grassLocalNormal, vec3(0.0, 1.0, 0.0), distanceBlend * 0.5);
    grassLocalNormal = normalize(grassLocalNormal);

    // Viewspace thicken
    vec4 mvPosition = view * model * vec4(grassLocalPosition, 1.0);
    vec3 viewDir = normalize(cameraPosition - grassBladeWorldPos);
    vec3 grassFaceNormal = (grassMat * vec3(0.0, 0.0, -zSide));
    float viewDotNormal = saturate(dot(grassFaceNormal, viewDir));
    float viewSpaceThickenFactor = easeOut(1.0 - viewDotNormal, 4.0) * smoothstep(0.0, 0.2, viewDotNormal);

    mvPosition.x += viewSpaceThickenFactor * (xSide - 0.5) * width * 0.5 * -zSide;

    gl_Position = projection * mvPosition;
    FragmentPosition = (model * vec4(grassLocalPosition, 1.0)).xyz;
    VertexColor = mix(c1, c2, smoothstep(-1.0, 1.0, noiseValue));
    VertexNormal = normalize(model * vec4(grassLocalNormal, 0.0)).xyz;
    GrassData = vec4(x, heightPercent, 0.0, 0.0);
}