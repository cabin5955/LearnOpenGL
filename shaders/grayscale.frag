#version 330 core

in vec2 TexCoord;
uniform sampler2D ourTexture;
out vec4 FragColor;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
}