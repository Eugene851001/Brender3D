#include "Mesh.h"
#include <iostream>

void Mesh::setup() 
{
	GLfloat* myVertices = (GLfloat*)&vertices[0]; 
	GLfloat vertex = myVertices[3];
	int size = vertices.size();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat) * 3, myVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

    glBindVertexArray(0);
}

Mesh::Mesh()
{
	VAO = EBO = VBO = -1;
	meshType = MeshType::Mesh;
	position = glm::vec3(0, 0, 0);
}

void Mesh::loadFromFile(const char* fileName)
{
	float x, y, z;
	FILE* f = fopen(fileName, "r");
	if (f)
	{
		char line[127];
		int i = 0;
		int c = 1;
		char junk;
		do
		{
			c = fgetc(f);
			line[i++] = (char)c;
			if (c == 10)
			{
				if (line[0] == 'v')
				{
					float x, y, z;
					sscanf(line, "%c %f %f %f", &junk, &x, &y, &z);
					vertices.push_back(glm::vec3(x, y, z));
				}
				if (line[0] == 'f')
				{
					int p1, p2, p3;
					sscanf(line, "%c %d %d %d", &junk, &p1, &p2, &p3);
					indices.push_back(p1);
					indices.push_back(p2);
					indices.push_back(p3);
				}
				i = 0;
			}
		} while (c != EOF);
		fclose(f);
	}
}

int Mesh::getVAO()
{
	return VAO;
}

MeshType Mesh::getType()
{
	return meshType;
}
