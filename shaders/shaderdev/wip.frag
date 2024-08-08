#version 330 core
out vec4 FragmentColor;

in vec2 TextureCoordinates;
vec2 uvs = TextureCoordinates;

uniform vec2 resolution;
uniform float time;

vec3 RED = vec3(1.0, 0.0, 0.0);
vec3 GREEN = vec3(0.0, 1.0, 0.0);
vec3 BLUE = vec3(0.0, 0.0, 1.0);
vec3 WHITE = vec3(1.0, 1.0, 1.0);
vec3 BLACK = vec3(0.0, 0.0, 0.0);
vec3 YELLOW = vec3(1.0, 1.0, 0.0);

float inverseLerp(float v, float minValue, float maxValue)
{
    return (v - minValue) / (maxValue - minValue);
}

float remap(float v, float inMin, float inMax, float outMin, float outMax)
{
    float t = inverseLerp(v, inMin, inMax);
    return mix(outMin, outMax, t);
}

vec3 BackgroundColor()
{
    float distFromCenter = length(abs(uvs - 0.5));

    float vignette = 1.0 - distFromCenter;
    vignette = smoothstep(0.0, 0.7, vignette);
    vignette = remap(vignette, 0.0, 1.0, 0.3, 1.0);

    return vec3(vignette);
}

vec3 DrawGrid(vec3 color, vec3 lineColor, float cellSpacing, float lineWidth)
{
    vec2 center = uvs - 0.5;
    vec2 cells = abs(fract(center * resolution / vec2(cellSpacing)) - 0.5);
    float distToEdge = (0.5 - max(cells.x, cells.y)) * cellSpacing;
    float lines = smoothstep(0.0, lineWidth, distToEdge);

    color = mix(lineColor, color, lines);

    return color;
}

float sdfCircle(vec2 p, float r)
{
    return length(p) - r;
}

float sdfLine(vec2 p, vec2 a, vec2 b)
{
    vec2 pa = p - a;
    vec2 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);

    return length(pa - ba * h);
}

float sdfBox(vec2 p, vec2 b)
{
    vec2 d = abs(p) - b;

    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

float sdfHexagon(vec2 p, float r)
{
    vec3 k = vec3(-0.866025404, 0.5, 0.577350269);
    p = abs(p);
    p -= 2.0 * min(dot(k.xy, p), 0.0) * k.xy;
    p -= vec2(clamp(p.x, -k.z * r, k.z * r), r);

    return length(p) * sign(p.y);
}

mat2 rotate2D(float angle)
{
    float sn = sin(angle);
    float cs = cos(angle);

    return mat2(cs, -sn, sn, cs);
}

void main()
{
    vec2 pixelCoords = (uvs - 0.5) * resolution;

    vec3 color = BackgroundColor();
    color = DrawGrid(color, vec3(0.5), 10.0, 1.0);
    color = DrawGrid(color, vec3(0.0), 100.0, 2.0);

    float d = sdfCircle(pixelCoords, 400.0);
    float d2 = sdfHexagon(pixelCoords, 200.0);

    d = mix(d, d2, remap(sin(time * 3), -1.0, 1.0, 0.0, 1.0));
    color = mix(RED * 0.25, color, smoothstep(-1.0, 1.0, d));
    color = mix(RED, color, smoothstep(-5.0, 0.0, d));

    FragmentColor = vec4(color, 1.0);
}