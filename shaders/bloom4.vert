#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Normals (not necessarily normalized)
layout (location = 1) in vec3 aNormal;
// Colors
layout (location = 2) in vec3 aColor;
// Texture Coordinates
layout (location = 3) in vec2 aTex;

out vec3 Normal;
out vec3 color;
out vec2 texCoord;

// Imports the camera matrix
uniform mat4 view;
uniform mat4 projection;
// Imports the transformation matrices
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	Normal = aNormal;
	color = aColor;
	texCoord = aTex;
}