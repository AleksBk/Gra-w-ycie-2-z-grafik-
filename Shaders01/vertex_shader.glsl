#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
 
//argument do wys³ania do fragment shadera.
out vec3 pos;
out vec4 colorOut;

//argumenty pochodz¹ce z poziomu hosta (CPU)
uniform mat4 MVP;
uniform vec4 color;
uniform float height;

void main()
{	
	colorOut = color;
	vec3 pos1 = vertexPosition_modelspace;
	pos1.y = height;
	gl_Position = MVP * vec4(pos1, 1); 	
	pos = pos1;
}