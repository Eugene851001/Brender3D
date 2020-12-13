#include "MeshNormalsTextures.h"

MeshNormalsTextures::MeshNormalsTextures()
{
	meshType = MeshType::MeshNormalsTextured;
}

void MeshNormalsTextures::loadFromFile(const char* fileName)
{
	vertices.clear();
	normals.clear();
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

					normalsIndices.push_back(n1);
					normalsIndices.push_back(n2);
					normalsIndices.push_back(n3);

					textureIndices.push_back(t1);
					textureIndices.push_back(t2);
					textureIndices.push_back(t3);
				}
				if (line[0] == 'v' && line[1] == 'n')
				{
					float x, y, z;
					sscanf(line + 2, "%f %f %f", &x, &y, &z);
					normals.push_back(glm::vec3(x, y, z));
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

void MeshNormalsTextures::setup()
{
	verticesNormalsTextures.clear();
	for (int i = 0; i < normalsIndices.size(); i++)
	{
		verticesNormalsTextures.push_back(vertices[indices[i] - 1].x);
		verticesNormalsTextures.push_back(vertices[indices[i] - 1].y);
		verticesNormalsTextures.push_back(vertices[indices[i] - 1].z);

		verticesNormalsTextures.push_back(normals[normalsIndices[i] - 1].x);
		verticesNormalsTextures.push_back(normals[normalsIndices[i] - 1].y);
		verticesNormalsTextures.push_back(normals[normalsIndices[i] - 1].z);

		verticesNormalsTextures.push_back(textureCoordinates[textureIndices[i] - 1].x);
		verticesNormalsTextures.push_back(textureCoordinates[textureIndices[i] - 1].y);

	}
	vertices.clear();
	textureCoordinates.clear();
	GLfloat* myVertices = (GLfloat*)&verticesNormalsTextures[0];
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesNormalsTextures.size() * sizeof(GLfloat), myVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}
