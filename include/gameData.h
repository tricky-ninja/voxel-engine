#pragma once
#include "Renderer.h"
#include <unordered_map>
#include <tuple>

const int CHUNK_SIZE = 16;
const int CHUNK_SIZE_VERTICAL = 255;
const unsigned RENDER_DISTANCE = 2;

struct Chunk
{
	glm::vec3 position = glm::vec3(0,0,0);
	bool data[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE_VERTICAL];
	Mesh mesh;

	bool blockAt(unsigned x, unsigned y, unsigned z);
	void setBlock(bool value, unsigned x, unsigned y, unsigned z);

	void generateMesh();
};
