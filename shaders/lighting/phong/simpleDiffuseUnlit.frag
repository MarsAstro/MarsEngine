#version 330 core
struct Material 
{
    sampler2D texture_diffuse1;
};

uniform Material material;

out vec4 FragmentColor;

in vec3 VertexNormal;
in vec3 FragmentPosition;
in vec2 TextureCoordinates;

void main()
{
    FragmentColor = texture(material.texture_diffuse1, TextureCoordinates);
}