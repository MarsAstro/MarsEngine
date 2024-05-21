#version 330 core
layout (location = 0) in vec2 vertexPos;
layout (location = 1) in vec3 vertexColor;

out VS_OUT {
    vec3 color;
} vs_out;

void main()
{
    gl_Position = vec4(vertexPos, 0.0,  1.0);
    vs_out.color = vertexColor;
}