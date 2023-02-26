#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTex;
layout (location=2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float scale;

out vec2 TexCoord;
out vec3 Normal;

void main()
{
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0f) + vec4(scale*Normal, 0);
    TexCoord = aTex;
}