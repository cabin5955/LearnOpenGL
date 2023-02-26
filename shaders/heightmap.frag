#version 330 core

in vec2 TexCoord;
in vec3 NewPos;
uniform sampler2D tex1;
uniform sampler2D tex2;
out vec4 FragColor;

void main()
{
    int m=2;
    if(NewPos.y > m)
    {
        FragColor = texture(tex1, TexCoord);
    }
    else
    {
        float alpha = NewPos.y/m;
        FragColor = texture(tex1, TexCoord)*alpha + texture(tex2, TexCoord)*(1.0-alpha);
    }
}