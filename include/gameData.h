#pragma once
#include "Renderer.h"
#include <map>
#include <tuple>
#include <deque>

const int CHUNK_SIZE = 16;
const int CHUNK_SIZE_VERTICAL = 256;
const int RENDER_DISTANCE = 16;

enum BlockType
{
	AIR_BLOCK,
	GRASS_BLOCK,
	DIRT_BLOCK,
	WATER_BLOCK,
	SAND_BLOCK,
	STONE_BLOCK,
	SNOW_BLOCK,
	BLOCK_TYPE_COUNT,
};

typedef BlockType BlockData;

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
	Mesh waterMesh;

	BlockData getBlockAt(int x, unsigned y, int z);
	void setBlockAt(BlockData value, unsigned x, unsigned y, unsigned z);

	void generateMesh();
};

struct World
{
	std::map<std::tuple<int, int>, Chunk*> chunks;
	std::vector<std::tuple<int, int>> sortedChunkIndicies;
	std::deque<std::tuple<int, int>> chunkQueue;
	std::vector<std::tuple<int, int>> chunksToDelete;

	int lastX = 0;
	int lastZ = 0;

	bool firstLoad = true;

	Chunk* getChunk(int x, int z);
	void render(const Shader &shader, const Camera &camera);
	void generateChunkData(int chunkX, int chunkZ);
	void applyUpdates();
	void updateState(glm::vec3 currentPos);
	void deleteAll();
};

glm::vec3 getBlockTextureID(BlockType block);
