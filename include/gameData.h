#pragma once
#include "Renderer.h"
#include <map>
#include <tuple>
#include <queue>

const int CHUNK_SIZE = 16;
const int CHUNK_SIZE_VERTICAL = 128;
const int RENDER_DISTANCE = 10;

typedef bool BlockData;

struct Chunk
{
	glm::vec3 position = glm::vec3(0,0,0);
	BlockData data[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE_VERTICAL];
	bool dirty = true;

	Chunk *left;
	Chunk *right;
	Chunk *front;
	Chunk *back;

	Mesh mesh;

	BlockData blockAt(int x, unsigned y, int z);
	void setBlock(BlockData value, unsigned x, unsigned y, unsigned z);

	void generateMesh();
};

struct ChunkManager
{
	std::map<std::tuple<int, int>, Chunk> chunks;
	int lastX = 0;
	int lastZ = 0;

	bool firstLoad = true;

	Chunk* getChunk(int x, int z);
	void render(const Shader &shader, const Camera &camera);
	void generate(int chunkX, int chunkZ);
	void update(glm::vec3 currentPos);
};
