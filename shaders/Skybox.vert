#version 330 core
layout (location = 0) in vec3 position;
out vec3 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	vec4 pos = projection * view * vec4(position.x, position.y, position.z, 1.0);
	gl_Position = pos.xyww;
	TexCoord = position;
} 