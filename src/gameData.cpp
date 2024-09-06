#include "gameData.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Logger.h"

std::vector<Vertex> cube = {
{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f)},
{ glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f)},
{ glm::vec3(0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f)},
{ glm::vec3(0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f)},
{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f)},
{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f)},

{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f)},
{ glm::vec3(0.5f, -0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f)},
{ glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f)},
{ glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f)},
{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f)},
{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f)},

{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f)},
{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f)},
{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f)},
{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f)},
{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f)},
{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f)},

{ glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(1.0f,  0.0f,  0.0f)},
{ glm::vec3(0.5f,  0.5f, -0.5f),  glm::vec3(1.0f,  0.0f,  0.0f)},
{ glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(1.0f,  0.0f,  0.0f)},
{ glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(1.0f,  0.0f,  0.0f)},
{ glm::vec3(0.5f, -0.5f,  0.5f),  glm::vec3(1.0f,  0.0f,  0.0f)},
{ glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(1.0f,  0.0f,  0.0f)},

{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f)},
{ glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(0.0f, -1.0f,  0.0f)},
{ glm::vec3(0.5f, -0.5f,  0.5f),  glm::vec3(0.0f, -1.0f,  0.0f)},
{ glm::vec3(0.5f, -0.5f,  0.5f),  glm::vec3(0.0f, -1.0f,  0.0f)},
{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f)},
{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f)},

{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f)},
{ glm::vec3(0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  1.0f,  0.0f)},
{ glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  1.0f,  0.0f)},
{ glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  1.0f,  0.0f)},
{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f)},
{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f)},
};

bool Chunk::blockAt(unsigned x, unsigned y, unsigned z)
{
	if ((x > 63) || (y > 63) || (z > 63)) return false;
	unsigned index = x | (z << 6) | (y << 12);
	return data[index];
}

void Chunk::setBlock(bool value, unsigned x, unsigned y, unsigned z)
{
	if ((x > 63) || (y > 63) || (z > 63))
		permAssert(false);
	unsigned index = x | (z << 6) | (y << 12);
	data[index] = value;
}

Mesh Chunk::generateMesh()
{
	Mesh mesh;
	const glm::vec3 offsets[] = {
		{0, 0, -1},  // back face
		{0, 0, 1},   // front face
		{-1, 0, 0},  // left face
		{1, 0, 0},   // right face
		{0, -1, 0},  // bottom face
		{0, 1, 0}    // top face
	};

	for (int x = 0; x < 64; ++x)
	{
		for (int z = 0; z < 64; ++z)
		{
			for (int y = 0; y < 64; ++y)
			{
				if (!blockAt(x, y, z))
					continue;  // Skip empty blocks

				for (unsigned face = 0; face < 6; ++face)
				{
					glm::vec3 neighborPos = glm::vec3(x, y, z) + offsets[face];

					// Skip faces where the neighbor block exists
					if (blockAt(neighborPos.x, neighborPos.y, neighborPos.z))
						continue;

					for (unsigned vertex = 0; vertex < 6; ++vertex)
					{
						Vertex newVertex;
						newVertex.position = cube[face * 6 + vertex].position + glm::vec3(x, y, z);
						newVertex.normal = cube[face * 6 + vertex].normal;
						mesh.vertices.push_back(newVertex);
					}
				}
			}
		}
	}
	mesh.setup();

	return mesh;
}