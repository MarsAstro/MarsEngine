#version 330 core
out vec4 FragmentColor;

in vec2 vertexUVs;

void main()
{
    FragmentColor = vec4(vertexUVs, 0.0, 1.0);
}