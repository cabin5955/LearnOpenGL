#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTex;
layout (location=2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D dep;

out vec2 TexCoord;
out vec3 NewPos;

void main()
{
    float height = texture(dep, aTex).r;
    vec3 newPosition = vec3(aPos.x, height*5, aPos.z);
    gl_Position = projection * view * model * vec4(newPosition, 1.0f);
    NewPos = newPosition;
    TexCoord = aTex;
}