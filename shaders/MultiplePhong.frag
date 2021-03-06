#version 330 core
out vec4 color;

struct LightSource
{
	vec3 LightPos;
	vec3 lightColor;
};

#define MAX_LIGHTS_SOURCE 10
uniform LightSource lightSources[MAX_LIGHTS_SOURCE];

in vec3 Normal;
in vec3 FragPos;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform int currentLightsAmount;

uniform int specLevel;

vec3 CalcPointLight(vec3 LightPos, vec3 lightColor, vec3 normal, vec3 fragPos, vec3 viewPos)
{
	vec3 ambient = objectColor * 0.1;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(LightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0);
	vec3 diffuse = diff * lightColor;
	
	float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specLevel);
    vec3 specular = specularStrength * spec * lightColor;  
		
	return ambient + (specular + diff) * objectColor;
}

void main()
{
	vec3 result = vec3(0.0, 0.0, 0.0);
	for(int i = 0; i < currentLightsAmount; i++)
	{
		result += CalcPointLight(lightSources[i].LightPos, lightSources[i].lightColor, Normal, FragPos, viewPos);
	}
	
	color = vec4(result, 1);
} 