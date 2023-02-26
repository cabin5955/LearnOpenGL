#version 330 core

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_emission1;
    float shininess;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D ourTexture;
uniform Light light;
uniform Material material;
uniform vec3 viewPos;

void main()
{
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));

    vec3  lightDir = normalize(light.position - FragPos);
    vec3  nor = normalize(Normal);
    float diff = max(dot(nor, lightDir), 0.0);
    vec3  diffuse = diff * light.diffuse * vec3(texture(material.texture_diffuse1, TexCoord));

    vec3  viewDir = normalize(viewPos - FragPos);
    vec3  reflectDir = reflect(-lightDir, nor);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3  specular = spec * light.specular * vec3(texture(material.texture_specular1, TexCoord));
    FragColor =  vec4(ambient + diffuse + specular, 1.0);
}