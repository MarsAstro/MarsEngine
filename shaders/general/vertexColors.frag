#version 330 core
out vec4 FragmentColor;

in vec3 vertexColor;

void main()
{
    FragmentColor = vec4(vertexColor, 1.0);
} 