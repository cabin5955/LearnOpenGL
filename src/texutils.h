#ifndef _TEXUTILS_H_
#define _TEXUTILS_H_

#include <stdio.h>
#include <string>

#include <assimp/texture.h>

using namespace std;

unsigned int replaceAll(string& str, const string& from, const string& to);
unsigned int loadTexture(char const * path);
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);
unsigned int LoadTextureFromAssImp(const aiTexture* aiTex);

#endif