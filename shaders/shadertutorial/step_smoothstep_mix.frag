#version 330 core
out vec4 FragmentColor;

in vec2 TextureCoordinates;
uniform sampler2D diffuse;
uniform sampler2D overlay;

void main()
{
    vec2 uvs = TextureCoordinates;
    vec3 color = vec3(0.0);

    float value1 = uvs.x;
    float value2 = smoothstep(0.0, 1.0, uvs.x);
    float value3 = step(0.5, uvs.x);

    float topLine = smoothstep(0.0, 0.005, abs(uvs.y - 0.66));
    float bottomLine = smoothstep(0.0, 0.005, abs(uvs.y - 0.33));
    float lines = topLine * bottomLine;

    float linearLine = smoothstep(0.0, 0.0075, abs(uvs.y - mix(0.33, 0.66, value1)));
    float smoothLine = smoothstep(0.0, 0.0075, abs(uvs.y - mix(0.0, 0.33, value2)));

    vec3 red = vec3(1.0, 0.0, 0.0);
    vec3 blue = vec3(0.0, 0.0, 1.0);
    vec3 lineColor = vec3(0.0, 0.0, 0.0);

    color = vec3(lines);

    if (uvs.y > 0.66)
    {
        color = mix(red, blue, step(0.5, value1));
    }
    else if (uvs.y > 0.33)
    {
        color = mix(red, blue, value1);
    }
    else
    {
        color = mix(red, blue, smoothstep(0.0, 1.0, value1));
    }

    color = mix(lineColor, color, lines);
    color = mix(lineColor, color, linearLine);
    color = mix(lineColor, color, smoothLine);

    FragmentColor = vec4(color, 1.0);
} 