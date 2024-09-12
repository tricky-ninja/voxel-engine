#pragma once
#include "Renderer.h"
#include <map>
#include <tuple>
#include <queue>

const int CHUNK_SIZE = 16;
const int CHUNK_SIZE_VERTICAL = 255;
const int RENDER_DISTANCE = 20;

struct Chunk
{
	glm::vec3 position = glm::vec3(0,0,0);
	bool data[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE_VERTICAL];
	Mesh mesh;

	bool blockAt(unsigned x, unsigned y, unsigned z);
	void setBlock(bool value, unsigned x, unsigned y, unsigned z);

	void generateMesh();
};

struct World
{
	std::map<std::tuple<int, int>, Chunk> chunks;
	glm::vec3 currentPos = { 0,0,0 };

	bool* getChunkData(int x, int z);
	void render(const Shader &shader, const Camera &camera);
	void generate(int chunkX, int chunkZ);
	void update();
};
