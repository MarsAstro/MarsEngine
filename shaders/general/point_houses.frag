#version 330 core
out vec4 FragmentColor;

in vec3 fragmentColor;

void main()
{
    FragmentColor = vec4(fragmentColor, 1.0);
} 