#version 330 core
out vec4 FragmentColor;

uniform vec3 objectColor;

void main()
{
    FragmentColor = vec4(objectColor, 1.0);
} 