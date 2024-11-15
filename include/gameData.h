#pragma once
#include "Renderer.h"
#include <map>
#include <tuple>
#include <deque>

const int CHUNK_SIZE = 16;
const int CHUNK_SIZE_VERTICAL = 256;


enum BlockType
{
	AIR_BLOCK,
	GRASS_BLOCK,
	DIRT_BLOCK,
	WATER_BLOCK,
	SAND_BLOCK,
	STONE_BLOCK,
	SNOW_BLOCK,
	RED_FLOWER,
	YELLOW_FLOWER,
	BLOCK_TYPE_COUNT,
};

enum BlockCategory
{
	SOLID,
	TRANSPARENT,
	WATER,
	BLOCK_CATEGORY_COUNT,
};

enum MeshType
{
	CUBE_MESH,
	CROSS_MESH,
	MESH_TYPE_COUNT,
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
	Mesh transparentMesh;

	BlockData get_block_at(int x, unsigned y, int z);
	void set_block(BlockData value, unsigned x, unsigned y, unsigned z);

	void generate_mesh();
};

struct World
{
	std::map<std::tuple<int, int>, Chunk*> chunks;
	std::vector<std::tuple<int, int>> sortedChunkIndicies;
	std::deque<std::tuple<int, int>> chunkQueue;
	std::vector<std::tuple<int, int>> chunksToDelete;

	int RENDER_DISTANCE = 16;
	int maxChunksPerFrame = 8;

	int lastX = 0;
	int lastZ = 0;

	bool firstLoad = true;

	Chunk* get_chunk(int x, int z);
	void render(const Shader &terrainShader, const Shader &waterShader, const Camera &camera);
	void generate_chunk_data(int chunkX, int chunkZ);
	void apply_updates();
	void update_state(glm::vec3 currentPos);
	void delete_all();
};

glm::vec3 get_block_textureID(BlockType block);
