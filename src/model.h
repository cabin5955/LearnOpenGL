#ifndef _MODEL_H_
#define _MODEL_H_

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shader.h"
#include "mesh.h"
#include <string>
#include <vector>

using namespace std;

class Model
{
    public:
        vector<Texture> textures_loaded;
        vector<Mesh> meshes;
        Model(const char *path);
        void Draw(Shader &shader);
    
    private:
        string directory;

        void loadModel(string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, 
                                             string typeName, const aiScene *scene);
};

#endif