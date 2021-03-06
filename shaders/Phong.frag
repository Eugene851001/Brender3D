#version 330 core
out vec4 color;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 LightPos;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform int specLevel;

void main()
{
	vec3 ambient = objectColor * 0.3;
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0);
	vec3 diffuse = diff * lightColor;
	
	float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specLevel);
    vec3 specular = specularStrength * spec * lightColor;  
	
	color = vec4(ambient + (specular + diff) * objectColor, 1);
} 