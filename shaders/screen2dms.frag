#version 330 core
out vec4 fragmentColor;

in vec2 texCoord;

//notice the sampler
uniform sampler2DMS screencapture;
				
void main(){
//texelFetch requires a vec of ints for indexing (since we're indexing pixel locations)
//texture coords is range [0, 1], we need range [0, viewport_dim].
//texture coords are essentially a percentage, so we can multiply text coords by total size 
    int viewport_width = 800; 
    int viewport_height = 600;
    ivec2 vpCoords = ivec2(viewport_width, viewport_height);
    vpCoords.x = int(vpCoords.x * texCoord.x); 
    vpCoords.y = int(vpCoords.y * texCoord.y);

//do a simple average since this is just a demo
	vec4 sample1 = texelFetch(screencapture, vpCoords, 0);
	vec4 sample2 = texelFetch(screencapture, vpCoords, 1);
	vec4 sample3 = texelFetch(screencapture, vpCoords, 2);
	vec4 sample4 = texelFetch(screencapture, vpCoords, 3);
	fragmentColor = (sample1 + sample2 + sample3 + sample4) / 4.0f;
}