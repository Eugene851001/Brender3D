#include "MeshNormals.h"

void MeshNormals::loadFromFile(const char* fileName)
{
	vertices.clear();
	normals.clear();
	indices.clear();
	normalsIndices.clear();
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
				if (line[0] == 'v' && line[1] != 'n')
				{
					float x, y, z;
					sscanf(line, "%c %f %f %f", &junk, &x, &y, &z);
					vertices.push_back(glm::vec3(x, y, z));
				}
				if (line[0] == 'f')
				{
					int p1, p2, p3;
					int n1, n2, n3;
					sscanf(line, "%c %d%c%c%d %d%c%c%d %d%c%c%d", &junk, &p1, &junk, &junk, &n1, 
						&p2, &junk, &junk, &n2, &p3, &junk, &junk, &n3);
					indices.push_back(p1);
					indices.push_back(p2);
					indices.push_back(p3);
					normalsIndices.push_back(n1);
					normalsIndices.push_back(n2);
					normalsIndices.push_back(n3);
				}
				if (line[0] == 'v' && line[1] == 'n')
				{
					float x, y, z;
					sscanf(line + 2, "%f %f %f", &x, &y, &z);
					normals.push_back(glm::vec3(x, y, z));
				}
				i = 0;
			}
		} while (c != EOF);
		fclose(f);
	}
}

void MeshNormals::setup()
{
	verticesNormals.clear();
	if (indices.size() != normalsIndices.size()
		|| normalsIndices.size() != normalsIndices.size())
	{
		throw "Can not load model";
	}

	if (normals.size() == 0)
	{
		throw "Can not load model";
	}

	for (int i = 0; i < normalsIndices.size(); i++)
	{
		if (indices[i] > vertices.size() || indices[i] < 0)
			throw "Can not load model";
		verticesNormals.push_back(vertices[indices[i] - 1]);
		if (normalsIndices[i] > verticesNormals.size() || normalsIndices[i] < 0)
			throw "Can not load model";
		verticesNormals.push_back(normals[normalsIndices[i] - 1]);
	}
	vertices.clear();
	normals.clear();
	GLfloat* myVertices = (GLfloat*)&verticesNormals[0];
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesNormals.size() * sizeof(GLfloat) * 3, myVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	glBindVertexArray(0); 
}

MeshNormals::MeshNormals()
{
	meshType = MeshType::MeshNormals;
}
