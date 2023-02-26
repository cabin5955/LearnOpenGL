#ifndef _FLOOR_H_
#define _FLOOR_H_

#include "shader.h"

class Floor 
{
public:
    Floor(Shader &shader, glm::vec3 position, unsigned int width, unsigned int height, unsigned int texture);
    ~Floor();
    void Draw();

private:
    unsigned int VAO, VBO;
    unsigned texture;
    Shader shader;
    glm::vec3 position;
    unsigned int verticesCount;
};

#endif