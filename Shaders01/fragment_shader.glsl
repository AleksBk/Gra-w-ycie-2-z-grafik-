#version 330 core 

out vec4 color;

//argument pochodz¹cy z Vertex_Shadera
in vec3 pos;
in vec4 colorOut;

void main()
{
	color = colorOut;
	color.r = pos.y*2.0;
}