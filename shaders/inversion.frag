#version 330 core

in vec2 TexCoord;
uniform sampler2D ourTexture;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vec3(1.0 - texture(ourTexture, TexCoord)), 1.0);
}