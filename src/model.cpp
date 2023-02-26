#include "model.h"
#include "texutils.h"

Model::Model(const char *path)
{
    loadModel(path);
}

void Model::Draw(Shader &shader)
{
    for(int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader); 
}

void Model::loadModel(string path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout<<"error::assimp::"<<import.GetErrorString()<<endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for(int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for(int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    for(int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vert;
        glm::vec3 v;
        v.x = mesh->mVertices[i].x;
        v.y = mesh->mVertices[i].y;
        v.z = mesh->mVertices[i].z;
        vert.Position = v;

        if(mesh->HasNormals())
        {
            v.x = mesh->mNormals[i].x;
            v.y = mesh->mNormals[i].y;
            v.z = mesh->mNormals[i].z;
            vert.Normal = v;
        }

        if(mesh->mTextureCoords[0])
        {
            glm::vec2 t;
            t.x = mesh->mTextureCoords[0][i].x;
            t.y = mesh->mTextureCoords[0][i].y;
            vert.TexCoords = t;
        }
        else
        {
            vert.TexCoords = glm::vec2(0.0f,0.0f);
        }
        vertices.push_back(vert);
    }

    for(int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    // 1. diffuse maps
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse",scene);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular",scene);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal",scene);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height",scene);
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());    

    return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, 
                                            string typeName, const aiScene *scene)
{
    vector<Texture> textures;
    for(int i = 0; i < mat->GetTextureCount(type); i++ )
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for(int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if(!skip)
        {
            Texture texture;
            auto aitexture = scene->GetEmbeddedTexture(str.C_Str());
            if(aitexture != nullptr)
            {
                cout<<"texture embeded "<<str.C_Str()<<endl;
                texture.id = LoadTextureFromAssImp(aitexture);
            }
            else
            {
                texture.id = TextureFromFile(str.C_Str(), this->directory);
            }
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }

    return textures;
}