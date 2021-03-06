#version 330 core
out vec4 color;
in vec3 Normal;
in vec3 FragPos;
uniform vec3 LightPos;
uniform vec3 ourColor;
void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0);
	color = vec4(diff * ourColor, 1);
} 