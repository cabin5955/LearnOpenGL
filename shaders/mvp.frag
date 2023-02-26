#version 330 core

in vec2 TexCoord;
out vec4 ourColor;

uniform sampler2D ourTexture;

void main()
{
    ourColor = texture(ourTexture, TexCoord);
}