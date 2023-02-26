#version 330 core
in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

uniform sampler2D ourTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    vec4 ambient = 0.1 * texture(ourTexture, TexCoord);

    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 nor = normalize(Normal);
    float diff = max(dot(nor, lightDir), 0.0);
    vec4 diffuse = diff * texture(ourTexture, TexCoord);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, nor);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec4 specular = spec * texture(ourTexture, TexCoord);

    FragColor = ambient + diffuse + specular;
}