#ifndef _CUBELIGHT_H_
#define _CUBELIGHT_H_

#include "shader.h"

class CubeLight {

public:
        CubeLight(Shader &shader);
        ~CubeLight();
        void Draw(glm::vec3 position, glm::vec3 color);

private:
        unsigned int VAO, VBO;
        Shader shader;            

};
#endif