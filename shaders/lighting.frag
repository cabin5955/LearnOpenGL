#version 330 core

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_emission1;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform float matrixMove;

void main()
{
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));

    vec3 lightDir = normalize(light.position - FragPos);
    vec3 nor = normalize(Normal);
    float diff = max(dot(nor,lightDir),0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoord));

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, nor);
    float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoord));

    vec3 emission = vec3(texture(material.texture_emission1, vec2(TexCoord.x,TexCoord.y + matrixMove)));

    FragColor = vec4(ambient + diffuse + specular + emission,1.0);
}
