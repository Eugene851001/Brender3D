#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 Normal;
out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
	Position = vec3(model * vec4(position.x, position.y, position.z, 1.0));
	Normal = mat3(transpose(inverse(model))) * normal;
} 