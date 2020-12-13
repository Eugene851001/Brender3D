#include "MeshTextured.h"

MeshTextured::MeshTextured()
{
	meshType = MeshType::MeshTextured;
	textureId = -1;
}

void MeshTextured::loadFromFile(const char* fileName)
{
	vertices.clear();
	indices.clear();
	textureCoordinates.clear();
	textureIndices.clear();
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
				if (line[0] == 'v' && line[1] != 'n' && line[1] != 't')
				{
					float x, y, z;
					sscanf(line, "%c %f %f %f", &junk, &x, &y, &z);
					vertices.push_back(glm::vec3(x, y, z));
				}
				if (line[0] == 'f')
				{
					int p1, p2, p3;
					int n1, n2, n3;
					int t1, t2, t3;

					sscanf(line, "%c %d%c%d%c%d %d%c%d%c%d %d%c%d%c%d", &junk, &p1, &junk, &t1, &junk, &n1,
						&p2, &junk, &t2, &junk, &n2, &p3, &junk, &t3, &junk, &n3);
					
					indices.push_back(p1);
					indices.push_back(p2);
					indices.push_back(p3);

					textureIndices.push_back(t1);
					textureIndices.push_back(t2);
					textureIndices.push_back(t3);
				}
				if (line[0] == 'v' && line[1] == 't')
				{
					float u, v;
					sscanf(line + 2, "%f %f", &u, &v);
					textureCoordinates.push_back(glm::vec2(u, v));
				}
				i = 0;
			}
		} while (c != EOF);
		fclose(f);
	}
}

void MeshTextured::setup()
{
	verticesTextures.clear();

	if (textureCoordinates.size() == 0)
	{
		throw "Can not load model";
	}

	for (int i = 0; i < indices.size(); i++)
	{
		verticesTextures.push_back(vertices[indices[i] - 1].x);
		verticesTextures.push_back(vertices[indices[i] - 1].y);
		verticesTextures.push_back(vertices[indices[i] - 1].z);

		verticesTextures.push_back(textureCoordinates[textureIndices[i] - 1].x);
		verticesTextures.push_back(textureCoordinates[textureIndices[i] - 1].y);

	}
	vertices.clear();
	textureCoordinates.clear();
	GLfloat* myVertices = (GLfloat*)&verticesTextures[0];
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesTextures.size() * sizeof(GLfloat), myVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}
