#version 330 core

// Outputs colors in RGBA
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BloomColor;

// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;


// Gets the Texture Units from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D normal0;
uniform sampler2D displacement0;

void main()
{
	// outputs final color
	FragColor = texture(diffuse0, texCoord);//pointLight();

	// Make the red lines of the lava brighter
	if (FragColor.r > 0.05f)
		FragColor.r *= 5.0f;

	// Calculate brightness by adding up all the channels with different weights each
	float brightness = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
    BloomColor = FragColor;//vec4(0.3f, 0.3f, 0.2f, 1.0f);
    // if(brightness > 0.15f)
    //     BloomColor = vec4(FragColor.rgb, 1.0f);
    // else
    //     BloomColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}