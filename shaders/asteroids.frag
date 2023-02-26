#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec4 ambient = 0.2 * texture(texture_diffuse, TexCoord);

    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 nor = normalize(Normal);
    float diff = max(dot(nor, lightDir), 0.0);
    vec4 diffuse = 0.5 * diff * texture(texture_diffuse, TexCoord);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, nor);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec4 specular = 0.3 * spec * texture(texture_diffuse, TexCoord);

    FragColor = ambient + diffuse + specular;
}
