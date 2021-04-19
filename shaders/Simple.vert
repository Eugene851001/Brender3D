#version 330 core
layout (location = 0) in vec3 position;
out vec4 ourColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
	ourColor = vec4(1, 0, 0, 1);
} 