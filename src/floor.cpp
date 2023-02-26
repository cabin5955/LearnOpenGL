#include "floor.h"
#include "mesh.h"

Floor::Floor(Shader &sha, glm::vec3 pos, unsigned int w, unsigned h, unsigned int t):shader(sha)
{
    position = pos;
    texture = t;

    int width = (int)w;
    int height = (int)h;

    std::vector<Vertex> vertices;
    float posY = 0.0f;
    for(int i = -width; i < width;i++)
    {
        for(int j = -height; j < height; j++)
        {
            Vertex vert;
            vert.Position  = glm::vec3(i, posY, j);
            vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
            vert.TexCoords = glm::vec2(0.0f, 1.0f);
            vertices.push_back(vert);

            vert.Position  = glm::vec3(i+1, posY, j);
            vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
            vert.TexCoords = glm::vec2(1.0f, 1.0f);
            vertices.push_back(vert);

            vert.Position  = glm::vec3(i+1, posY, j+1);
            vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
            vert.TexCoords = glm::vec2(1.0f, 0.0f);
            vertices.push_back(vert);

            vert.Position  = glm::vec3(i+1, posY, j+1);
            vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
            vert.TexCoords = glm::vec2(1.0f, 0.0f);
            vertices.push_back(vert);

            vert.Position  = glm::vec3(i, posY, j+1);
            vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
            vert.TexCoords = glm::vec2(0.0f, 0.0f);
            vertices.push_back(vert);

            vert.Position  = glm::vec3(i, posY, j);
            vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
            vert.TexCoords = glm::vec2(0.0f, 1.0f);
            vertices.push_back(vert);
        }
    }
    verticesCount =  vertices.size();
    std::cout<<"vertices count "<<verticesCount<<std::endl;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesCount*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
}

Floor::~Floor()
{

}

void Floor::Draw()
{
    shader.use();
    glm::mat4 model(1.0f);
    model = glm::translate(model, position);
    shader.setMat4("model", model);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, verticesCount);
}