#include "RenderStateMachine.h"
#include <string>
#include <Windows.h>
#include <SOIL.h>


void RenderStateMachine::initSkybox()
{
    mySkybox = new Skybox(this->skyboxImageData);
}

void RenderStateMachine::initDepthFBO()
{
    glCreateFramebuffers(1, &depthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw "Can not set framebuffer";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderStateMachine::init(int width, int height, HANDLE events[INIT_STEPS_AMOUNT])
{
    this->width = width;
    this->height = height;
    simpleShader = new Shader("shaders/Simple.vert", "shaders/Simple.frag");
    phongShader = new Shader("shaders/Phong.vert", "shaders/Phong.frag");
    skyboxShader = new Shader("shaders/Skybox.vert", "shaders/Skybox.frag");
    textureShader = new Shader("shaders/Texture.vert", "shaders/Texture.frag");
    texturePhongShader = new Shader("shaders/TexturePhong.vert", "shaders/TexturePhong.frag");
    bufferShader = new Shader("shaders/Buffer.vert", "shaders/Buffer.frag");
    shadowMapShader = new Shader("shaders/ShadowMapping.vert", "shaders/ShadowMapping.frag");
    shadowShader = new Shader("shaders/Shadow.vert", "shaders/Shadow.frag");
    multiplePhongShader = new Shader("shaders/Phong.vert", "shaders/MultiplePhong.frag");
    skyboxReflectionShader = new Shader("shaders/SkyboxReflection.vert", "shaders/SkyboxReflection.frag");

    lamp = new Ball(glm::vec3(0, 10, 0), glm::vec3(0, 0, 1), 0.5, 0, glm::vec3(1, 1, 1));
    lamp2 = new Ball(glm::vec3(0, -8, 0), glm::vec3(0, 1, 0), 0.5, 0, glm::vec3(1, 0, 0));

    lamps.push_back(lamp);
    lamps.push_back(new Ball(glm::vec3(0, -8, 0), glm::vec3(0, 1, 0), 0.5, 0, glm::vec3(1, 0, 0)));
    lamps.push_back(new Ball(glm::vec3(0, 0, 10), glm::vec3(0, 1, 0), 0.5, 0, glm::vec3(0, 1, 1)));

    const char* skyboxFiles[] = { "skybox/right.jpg" , "skybox/left.jpg", "skybox/top.jpg",
        "skybox/bottom.jpg", "skybox/front.jpg" , "skybox/back.jpg" };

    for (int i = 0; i < 6; i++)
    {
        skyboxImageData[i] = SOIL_load_image(skyboxFiles[i], &width, &height, 0, SOIL_LOAD_RGB);
        SetEvent(events[i]);
    }

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);


    camera = new Camera(glm::vec3(0, 0, 10), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));

    glEnable(GL_DEPTH_TEST);
}

RenderStateMachine::RenderStateMachine(int width, int height, HANDLE events[6])
{
    init(width, height, events);
}

RenderStateMachine::~RenderStateMachine()
{
    for (int i = 0; i < meshes.size(); i++)
    {
        delete meshes[i];
    }

    delete mySkybox;

    for (int i = 0; i < 6; i++)
    {
        delete skyboxImageData[i];
    }

    for (int i = 0; i < lamps.size(); i++)
    {
        delete lamps[i];
    }
}

void RenderStateMachine::Render()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (RenderMode & RENDER_LIGHTS)
    {
        renderLightSources();
    }
    if (RenderMode & RENDER_TEXTURED)
    {
        renderTexture();
        renderSimpleTexture();
    }
    if (RenderMode & RENDER_SKYBOX)
    {
        renderSkybox();
    }
    if (RenderMode & RENDER_REFLECTION)
    {
        renderSkyboxReflection();
    }
    if (RenderMode & RENDER_SHADOW)
    {
        renderShadowMap();
        renderShadow();
    }
    if (RenderMode & RENDER_PHONG)
    {
        renderPhong();
    }
    if (RenderMode & RENDER_MULTIPLEPHONG)
    {
        renderMultiplePhong();
    }
    if (RenderMode & RENDER_SIMPLE)
    {
        renderSimple();
    }
}

void RenderStateMachine::renderSkybox()
{
    skyboxShader->use();

    glm::mat4 modelMat;
    glm::mat4 projMat;
    glm::mat4 viewMat;

    GLuint modelLocation, projLocation, viewLocation;

    modelMat = glm::mat4(1.0f);
    projMat = glm::perspective(45.0f, (GLfloat)(width) / (GLfloat)(height), 0.1f, 100.0f);
    viewMat = camera->getLookAtForSkybox();

    modelLocation = glGetUniformLocation(skyboxShader->programId, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));

    projLocation = glGetUniformLocation(skyboxShader->programId, "projection");
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projMat));

    viewLocation = glGetUniformLocation(skyboxShader->programId, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMat));

    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(mySkybox->getVAO());
    glBindTexture(GL_TEXTURE_CUBE_MAP, mySkybox->getTextureId());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void RenderStateMachine::renderSkyboxReflection()
{
    skyboxReflectionShader->use();

    glm::mat4 modelMat;
    glm::mat4 projMat;
    glm::mat4 viewMat;

    modelMat = glm::mat4(1.0f);
    projMat = glm::perspective(45.0f, (GLfloat)(width) / (GLfloat)(height), 0.1f, 100.0f);
    viewMat = camera->getLookAt();

    GLuint modelLocation, viewLocation, projLocation, colorLocation;

    modelLocation = glGetUniformLocation(skyboxReflectionShader->programId, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));

    viewLocation = glGetUniformLocation(skyboxReflectionShader->programId, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMat));

    projLocation = glGetUniformLocation(skyboxReflectionShader->programId, "projection");
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projMat));

    GLuint viewPosLocation = glGetUniformLocation(skyboxReflectionShader->programId, "viewPos");
    glUniform3f(viewPosLocation, camera->getPosition().x, camera->getPosition().y,
        camera->getPosition().z);

    for (int i = 0; i < meshes.size(); i++)
    {
        if (meshes[i]->getType() == MeshType::MeshNormals || meshes[i]->getType() == MeshType::MeshNormalsTextured)
        {
            modelMat = glm::translate(glm::mat4(1.0f), meshes[i]->position);
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
            glBindVertexArray(meshes[i]->getVAO());
            glBindTexture(GL_TEXTURE_CUBE_MAP, mySkybox->getTextureId());
            glDrawArrays(GL_TRIANGLES, 0, meshes[i]->indices.size());
            glBindVertexArray(0);
        }
    }
}

void RenderStateMachine::renderTexture()
{
    texturePhongShader->use();

    glm::mat4 modelMat;
    glm::mat4 projMat;
    glm::mat4 viewMat;


    modelMat = glm::mat4(1.0f);
    projMat = glm::perspective(45.0f, (GLfloat)(width) / (GLfloat)(height), 0.1f, 100.0f);
    viewMat = camera->getLookAt();

    GLuint modelLocation, viewLocation, projLocation, colorLocation;
    modelLocation = glGetUniformLocation(texturePhongShader->programId, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));

    projLocation = glGetUniformLocation(texturePhongShader->programId, "projection");
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projMat));

    viewLocation = glGetUniformLocation(texturePhongShader->programId, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMat));

    GLuint lightPosLocation = glGetUniformLocation(texturePhongShader->programId, "LightPos");
    glUniform3f(lightPosLocation, lamp->getPosition().x, lamp->getPosition().y, lamp->getPosition().z);

    GLuint lightColorLocation = glGetUniformLocation(texturePhongShader->programId, "lightColor");
    glUniform3f(lightColorLocation, lamp->getColor().r, lamp->getColor().g, lamp->getColor().b);

    GLuint viewPosLocation = glGetUniformLocation(texturePhongShader->programId, "viewPos");
    glUniform3f(viewPosLocation, camera->getPosition().x, camera->getPosition().y,
        camera->getPosition().z);

    GLuint specLevelLocation = glGetUniformLocation(texturePhongShader->programId, "specLevel");
    glUniform1i(specLevelLocation, 8);

    for (int i = 0; i < meshes.size(); i++)
    {
        if (meshes[i]->getType() == MeshType::MeshNormalsTextured)
        {
            MeshTextured* mesh = static_cast<MeshTextured*>(meshes[i]);
            modelMat = glm::translate(glm::mat4(1.0f), mesh->position);
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
            glBindVertexArray(mesh->getVAO());
            glBindTexture(GL_TEXTURE_2D, mesh->textureId);
            glDrawArrays(GL_TRIANGLES, 0, mesh->indices.size());
            glBindVertexArray(0);
        }
    }

}

void RenderStateMachine::renderSimpleTexture()
{
    textureShader->use();

    glm::mat4 modelMat;
    glm::mat4 projMat;
    glm::mat4 viewMat;


    modelMat = glm::mat4(1.0f);
    projMat = glm::perspective(45.0f, (GLfloat)(width) / (GLfloat)(height), 0.1f, 100.0f);
    viewMat = camera->getLookAt();

    GLuint modelLocation, viewLocation, projLocation, colorLocation;
    modelLocation = glGetUniformLocation(textureShader->programId, "model");

    projLocation = glGetUniformLocation(textureShader->programId, "projection");
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projMat));

    viewLocation = glGetUniformLocation(textureShader->programId, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMat));

    for (int i = 0; i < meshes.size(); i++)
    {
        if (meshes[i]->getType() == MeshType::MeshTextured)
        {
            MeshTextured* mesh = static_cast<MeshTextured*>(meshes[i]);
            modelMat = glm::translate(glm::mat4(1.0f), mesh->position);
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
            glBindVertexArray(mesh->getVAO());
            glBindTexture(GL_TEXTURE_2D, mesh->textureId);
            glDrawArrays(GL_TRIANGLES, 0, mesh->indices.size());
            glBindVertexArray(0);
        }
    }
}

void RenderStateMachine::renderShadowMap ()
{
    shadowMapShader->use();
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 modelMat;
    glm::mat4 projMat;
    glm::mat4 viewMat;

    GLuint modelLocation, viewLocation, projLocation;

    modelMat = glm::mat4(1.0f);
    //projMat = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
    projMat = glm::perspective(45.0f, (GLfloat)(width) / (GLfloat)(height), 0.1f, 100.0f);
    //Attention! using light position
    viewMat = glm::lookAt(lamp->getPosition(), glm::vec3(0.0f), 
        glm::vec3(1.0, 0.0, 0.0));

    modelLocation = glGetUniformLocation(shadowMapShader->programId, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));

    viewLocation = glGetUniformLocation(shadowMapShader->programId, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMat));

    projLocation = glGetUniformLocation(shadowMapShader->programId, "projection");
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projMat));

    lightSpaceMatrix = projMat * viewMat;

    for (int i = 0; i < meshes.size(); i++)
    {
        if (meshes[i]->getType() == MeshType::MeshNormals)
        {
            modelMat = glm::translate(glm::mat4(1.0f), meshes[i]->position);
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
            glBindVertexArray(meshes[i]->getVAO());
            glDrawArrays(GL_TRIANGLES, 0, meshes[i]->indices.size());
            glBindVertexArray(0);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderStateMachine::renderShadow()
{
    shadowShader->use();

    glm::mat4 modelMat;
    glm::mat4 projMat;
    glm::mat4 viewMat;


    modelMat = glm::mat4(1.0f);
    projMat = glm::perspective(45.0f, (GLfloat)(width) / (GLfloat)(height), 0.1f, 100.0f);
    viewMat = camera->getLookAt();

    GLuint modelLocation, viewLocation, projLocation, colorLocation;

    modelLocation = glGetUniformLocation(shadowShader->programId, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));

    viewLocation = glGetUniformLocation(shadowShader->programId, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMat));

    projLocation = glGetUniformLocation(shadowShader->programId, "projection");
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projMat));

    colorLocation = glGetUniformLocation(shadowShader->programId, "objectColor");
    glUniform3f(colorLocation, 0.3, 0.3, 0.3);

    GLuint lightPosLocation = glGetUniformLocation(shadowShader->programId, "LightPos");
    glUniform3f(lightPosLocation, lamp->getPosition().x, lamp->getPosition().y, lamp->getPosition().z);

    GLuint lightColorLocation = glGetUniformLocation(shadowShader->programId, "lightColor");
    glUniform3f(lightColorLocation, lamp->getColor().r, lamp->getColor().g, lamp->getColor().b);

    GLuint viewPosLocation = glGetUniformLocation(shadowShader->programId, "viewPos");
    glUniform3f(viewPosLocation, camera->getPosition().x, camera->getPosition().y,
        camera->getPosition().z);

    GLuint lightSpaceMatLocatioin = glGetUniformLocation(shadowShader->programId, "lightSpaceMatrix");
    glUniformMatrix4fv(lightSpaceMatLocatioin, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    for (int i = 0; i < meshes.size(); i++)
    {
        if (meshes[i]->getType() == MeshType::MeshNormals)
        {
            modelMat = glm::translate(glm::mat4(1.0f), meshes[i]->position);
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
            glBindVertexArray(meshes[i]->getVAO());
            glDrawArrays(GL_TRIANGLES, 0, meshes[i]->indices.size());
            glBindVertexArray(0);
        }
    }
}

void RenderStateMachine::renderPhong()
{
    phongShader->use();

    glm::mat4 modelMat;
    glm::mat4 projMat;
    glm::mat4 viewMat;


    modelMat = glm::mat4(1.0f);
    projMat = glm::perspective(45.0f, (GLfloat)(width) / (GLfloat)(height), 0.1f, 100.0f);
    viewMat = camera->getLookAt();

    GLuint modelLocation, viewLocation, projLocation, colorLocation;

    modelLocation = glGetUniformLocation(phongShader->programId, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));

    viewLocation = glGetUniformLocation(phongShader->programId, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMat));

    projLocation = glGetUniformLocation(phongShader->programId, "projection");
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projMat));

    colorLocation = glGetUniformLocation(phongShader->programId, "objectColor");
    glUniform3f(colorLocation, 0.5, 0.5, 0.5);

    GLuint lightPosLocation = glGetUniformLocation(phongShader->programId, "LightPos");
    glUniform3f(lightPosLocation, lamp->getPosition().x, lamp->getPosition().y, lamp->getPosition().z);

    GLuint lightColorLocation = glGetUniformLocation(phongShader->programId, "lightColor");
    glUniform3f(lightColorLocation, lamp->getColor().r, lamp->getColor().g, lamp->getColor().b);

    GLuint viewPosLocation = glGetUniformLocation(phongShader->programId, "viewPos");
    glUniform3f(viewPosLocation, camera->getPosition().x, camera->getPosition().y,
        camera->getPosition().z);

    GLuint specLevelLocation = glGetUniformLocation(phongShader->programId, "specLevel");
    glUniform1i(specLevelLocation, 256);

    for (int i = 0; i < meshes.size(); i++)
    {
        if (meshes[i]->getType() == MeshType::MeshNormals || meshes[i]->getType() == MeshType::MeshNormalsTextured)
        {
            glUniform1i(specLevelLocation, meshes[i]->specLevel);
            modelMat = glm::translate(glm::mat4(1.0f), meshes[i]->position);
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
            glBindVertexArray(meshes[i]->getVAO());
            glDrawArrays(GL_TRIANGLES, 0, meshes[i]->indices.size());
            glBindVertexArray(0);
        }
    }

}

void RenderStateMachine::renderLightSources()
{
    simpleShader->use();

    glm::mat4 modelMat;
    glm::mat4 projMat;
    glm::mat4 viewMat;

    modelMat = glm::mat4(1.0f);
    projMat = glm::perspective(45.0f, (GLfloat)(width) / (GLfloat)(height), 0.1f, 100.0f);
    viewMat = camera->getLookAt();

    GLuint modelLocation, viewLocation, projLocation, colorLocation;

    modelLocation = glGetUniformLocation(simpleShader->programId, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));

    viewLocation = glGetUniformLocation(simpleShader->programId, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMat));

    projLocation = glGetUniformLocation(simpleShader->programId, "projection");
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projMat));

    colorLocation = glGetUniformLocation(simpleShader->programId, "ourColor");

    if (RenderMode & RENDER_PHONG || RenderMode & RENDER_SHADOW)
    {
        glUniform4f(colorLocation, lamps[0]->getColor().r, lamps[0]->getColor().g, lamps[0]->getColor().b, 1);

        modelMat = glm::translate(glm::mat4(1.0f), lamps[0]->getPosition());
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));

        glBindVertexArray(lamps[0]->getVAO());
        glDrawElements(GL_TRIANGLES, lamps[0]->getIndicesSize(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        return;
    }

    for (int i = 0; i < lamps.size(); i++)
    {
        glUniform4f(colorLocation, lamps[i]->getColor().r, lamps[i]->getColor().g, lamps[i]->getColor().b, 1);

        modelMat = glm::translate(glm::mat4(1.0f), lamps[i]->getPosition());
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));

        glBindVertexArray(lamps[i]->getVAO());
        glDrawElements(GL_TRIANGLES, lamps[i]->getIndicesSize(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

}

void RenderStateMachine::renderSimple()
{
    simpleShader->use();

    glm::mat4 modelMat;
    glm::mat4 projMat;
    glm::mat4 viewMat;

    modelMat = glm::mat4(1.0f);
    projMat = glm::perspective(45.0f, (GLfloat)(width) / (GLfloat)(height), 0.1f, 100.0f);
    viewMat = camera->getLookAt(); 

    GLuint modelLocation, viewLocation, projLocation, colorLocation;

    modelLocation = glGetUniformLocation(simpleShader->programId, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));

    viewLocation = glGetUniformLocation(simpleShader->programId, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMat));

    projLocation = glGetUniformLocation(simpleShader->programId, "projection");
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projMat));

    colorLocation = glGetUniformLocation(simpleShader->programId, "ourColor");
    glUniform4f(colorLocation, 1, 0, 0, 1);
    
    for (int i = 0; i < meshes.size(); i++)
    {
        modelMat = glm::translate(glm::mat4(1.0f), meshes[i]->position);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
        glBindVertexArray(meshes[i]->getVAO());
        glDrawArrays(GL_TRIANGLES, 0, meshes[i]->indices.size());
        glBindVertexArray(0);
    }
}

void RenderStateMachine::renderMultiplePhong()
{
    multiplePhongShader->use();

    glm::mat4 modelMat;
    glm::mat4 projMat;
    glm::mat4 viewMat;


    modelMat = glm::mat4(1.0f);
    projMat = glm::perspective(45.0f, (GLfloat)(width) / (GLfloat)(height), 0.1f, 100.0f);
    viewMat = camera->getLookAt();

    GLuint modelLocation, viewLocation, projLocation, colorLocation;

    modelLocation = glGetUniformLocation(multiplePhongShader->programId, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));

    viewLocation = glGetUniformLocation(multiplePhongShader->programId, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMat));

    projLocation = glGetUniformLocation(multiplePhongShader->programId, "projection");
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projMat));

    colorLocation = glGetUniformLocation(multiplePhongShader->programId, "objectColor");
    glUniform3f(colorLocation, 0.5, 0.5, 0.5);

    GLuint currentLightsAmountLocation = glGetUniformLocation(multiplePhongShader->programId, "currentLightsAmount");
    glUniform1i(currentLightsAmountLocation, (GLint)lamps.size());

    GLuint viewPosLocation = glGetUniformLocation(multiplePhongShader->programId, "viewPos");
    glUniform3f(viewPosLocation, camera->getPosition().x, camera->getPosition().y,
        camera->getPosition().z);

    GLuint specLevelLocation = glGetUniformLocation(multiplePhongShader->programId, "specLevel");

    GLuint lightColorLocation, lightPosLocation;
    for (int i = 0; i < lamps.size(); i++)
    {
        std::string str1 = "lightSources[";
        char num[5];
        _itoa(i, num, 10);
        str1.append(num);
        str1.append("].LightPos");
        lightPosLocation = glGetUniformLocation(multiplePhongShader->programId, str1.c_str());
        glUniform3f(lightPosLocation, lamps[i]->getPosition().x, lamps[i]->getPosition().y, lamps[i]->getPosition().z);

        std::string str2 = "lightSources[";
        str2.append(num);
        str2.append("].lightColor");
        lightColorLocation = glGetUniformLocation(multiplePhongShader->programId, str2.c_str());
        glUniform3f(lightColorLocation, lamps[i]->getColor().r, lamps[i]->getColor().g, lamps[i]->getColor().b);
    }

    for (int i = 0; i < meshes.size(); i++)
    {
        if (meshes[i]->getType() == MeshType::MeshNormals)
        {
            glUniform1i(specLevelLocation, meshes[i]->specLevel);
            modelMat = glm::translate(glm::mat4(1.0f), meshes[i]->position);
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
            glBindVertexArray(meshes[i]->getVAO());
            glDrawArrays(GL_TRIANGLES, 0, meshes[i]->indices.size());
            glBindVertexArray(0);
        }
    }

}

bool RenderStateMachine::isExistsNameMesh(const char* name)
{
    std::string strName(name);
    for (int i = 0; i < meshesNames.size(); i++)
    {
        if (static_cast<std::string>(meshesNames[i].first).compare(strName) == 0)
            return true;
    }
    return false;
}

bool RenderStateMachine::addMesh(Mesh* mesh, const char* name)
{
    if (isExistsNameMesh(name))
    {
        return false;
    }

    std::string strName(name);
    meshes.push_back(mesh);
    meshesNames.push_back(std::pair<std::string, int>(strName, meshes.size() - 1));
    return true;
}

bool RenderStateMachine::isExistsNameTexture(const char* name)
{
    std::string strName(name);
    for (int i = 0; i < texturesNames.size(); i++)
    {
        if (static_cast<std::string>(texturesNames[i].first).compare(strName) == 0)
            return true;
    }
    return false;
}

bool RenderStateMachine::addTexture(Texture texture, const char* name)
{
    if (isExistsNameTexture(name))
    {
        return false;
    }

    std::string strName(name);
    textures.push_back(texture);
    Texture* ptrTexture = &textures[textures.size() - 1];
    texturesNames.push_back(std::pair<std::string, int>(strName, textures.size() - 1));
    return true;
}

int RenderStateMachine::getMeshIndexByName(const char* name)
{
    std::string strName(name);
    int result = -1;
    for (int i = 0; i < meshesNames.size(); i++)
    {
        if (meshesNames[i].first.compare(strName) == 0)
        {
            result = meshesNames[i].second;
        }
    }
    return result;
}

std::vector<std::string> RenderStateMachine::getAllMeshesNames()
{
    std::vector<std::string> names;
    for (int i = 0; i < meshesNames.size(); i++)
    {
        names.push_back(meshesNames[i].first);
    }

    return names;
}

void RenderStateMachine::changeMeshPosition(int index, glm::vec3 position)
{
    if (index < 0 || index >= meshes.size())
        return;

    meshes[index]->position = position;
}

int RenderStateMachine::getTextureIndexByName(const char* name)
{
    std::string strName(name);
    int result = -1;
    for (int i = 0; i < texturesNames.size(); i++)
    {
        if (texturesNames[i].first.compare(strName) == 0)
        {
            result = texturesNames[i].second;
        }
    }
    return result;
}

std::vector<std::string> RenderStateMachine::getAllTexturesNames()
{
    std::vector<std::string> names;
    for (int i = 0; i < texturesNames.size(); i++)
    {
        names.push_back(texturesNames[i].first);
    }
    return names;
}

GLuint RenderStateMachine::getTextureIdByIndex(int index)
{
    if (index < 0 || index > (int)texturesNames.size())
    {
        return -1;
    }

    return textures[index].getId();
}

Mesh RenderStateMachine::getMeshInfo(int index)
{
    if (index <  0 || index > meshes.size())
        return Mesh();

    return *meshes[index];
}