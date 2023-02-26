#version 330 core

out vec4 FragColor;

in float height;

void main()
{
    float h = (height + 16) / 32.0;
    FragColor = vec4(h, h, h, 1.0);
}