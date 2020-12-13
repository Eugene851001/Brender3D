#pragma once

#define GLEW_STATIC
#include <glew.h>

#include "Shader.h"
#include "Skybox.h"
#include "Mesh.h"
#include "MeshNormals.h"
#include "MeshNormalsTextures.h"
#include "Texture.h"
#include "Ball.h"
#include "Camera.h"

#include <Windows.h>
#include <utility>
#include <string>

static float quadVertices[] = { 
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

class RenderStateMachine
{
	Shader* simpleShader;
	Shader* phongShader;
	Shader* skyboxShader;
	Shader* shadowMapShader;
	Shader* shadowShader;
	Shader* textureShader;
	Shader* texturePhongShader;
	Shader* bufferShader;
	Shader* multiplePhongShader;
	Shader* skyboxReflectionShader;

	unsigned char* skyboxImageData[6];
	Skybox* mySkybox;

	std::vector<std::pair<std::string, int>> meshesNames;
	std::vector<std::pair<std::string, int>> texturesNames;

	std::vector<Mesh*> meshes;
	std::vector<Texture> textures;

	Ball* lamp;
	Ball* lamp2;

	GLuint quadVAO, quadVBO;

	glm::mat4 lightSpaceMatrix;

	unsigned int depthFBO, depthMap;

	int width, height;

	bool isExistsNameMesh(const char* name);
	bool isExistsNameTexture(const char* name);

	void init(int width, int height, HANDLE events[6]);

	void renderShadowMap();
	void renderShadow();
	void renderPhong();
	void renderSkybox();
	void renderTexture();
	void renderLightSources();
	void renderMultiplePhong();
	void renderSimple();
	void renderSkyboxReflection();
	void renderSimpleTexture();
public:
	Camera* camera;
	static const int RENDER_SHADOW = 1;
	static const int RENDER_PHONG = 2;
	static const int RENDER_MULTIPLEPHONG = 4;
	static const int RENDER_SIMPLE = 8;
	static const int RENDER_SKYBOX = 16;
	static const int RENDER_TEXTURED = 32;
	static const int RENDER_REFLECTION = 64;
	static const int RENDER_LIGHTS = 128;

	int RenderMode = RENDER_SIMPLE;

	std::vector<Ball*> lamps;

	static const int INIT_STEPS_AMOUNT = 6;
	RenderStateMachine(int width, int heigt, HANDLE events[INIT_STEPS_AMOUNT]);
	~RenderStateMachine();
	void initSkybox();
	void initDepthFBO();
	void Render();

	bool addMesh(Mesh* mesh, const char* name);
	bool addTexture(Texture texture, const char* name);

	int getMeshIndexByName(const char* name);
	Mesh getMeshInfo(int index);
	std::vector<std::string> getAllMeshesNames();

	int getTextureIndexByName(const char* name);
	std::vector<std::string> getAllTexturesNames();
	GLuint getTextureIdByIndex(int index);

	void changeMeshPosition(int index, glm::vec3 position);
};

