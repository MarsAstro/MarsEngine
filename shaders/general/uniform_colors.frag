#version 330 core
out vec4 FragmentColor;

in vec2 vertexUVs;
uniform vec4 color1;
uniform vec4 color2;

void main()
{
    FragmentColor = mix
    (
        color1,
        color2,
        vertexUVs.x
    ); 
} 