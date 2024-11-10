#include "gameData.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Logger.h"

#include "Cube.h"

#pragma region STATIC_FUNCTIONS

static int calculateAO(bool side1, bool side2, bool corner) {
	if (side1 && side2) return 0;
	return 3 - (side1 + side2 + corner);
}

static float distanceSquared(const std::tuple<int, int>& a, const std::tuple<int, int>& b) {
	float dx = std::get<0>(a) - std::get<0>(b);
	float dz = std::get<1>(a) - std::get<1>(b);
	return dx * dx + dz * dz;
}


static glm::vec3 getBlockTextureID(BlockType block)
{
	glm::vec3 blockTextures[BLOCK_TYPE_COUNT - 1] = {
		glm::vec3(3,0,3), // GRASS_BLOCK
		glm::vec3(2,2,2), // DIRT_BLOCK
		glm::vec3(223,223,223),  // WATER_BLOCK
		glm::vec3(18,18,18),  // SAND_BLOCK
		glm::vec3(1,1,1),  // STONE_BLOCK
		glm::vec3(66,66,66),  // SNOW_BLOCK
	};

	if (block > 0 && block < BLOCK_TYPE_COUNT)
	{
		return blockTextures[block - 1];
	}
	return glm::vec3(14, 14, 14);	// return empty texture(pink color) if block id is invalid
}

#pragma endregion

#pragma region CHUNK_STUFF

BlockData Chunk::getBlockAt(int x, unsigned y, int z)
{
	// Boundary checks
	if (x < -1 || x > CHUNK_SIZE || z < -1 || z > CHUNK_SIZE || y >= CHUNK_SIZE_VERTICAL)
		return AIR_BLOCK;

	// Special corner cases: reject invalid boundary conditions
	if ((x == -1 && z == -1) || (x == -1 && z == CHUNK_SIZE) ||
		(x == CHUNK_SIZE && z == -1) || (x == CHUNK_SIZE && z == CHUNK_SIZE))
		return AIR_BLOCK;

	// Handle neighbor chunks
	if (x == -1 && left != nullptr)
		return left->data[(CHUNK_SIZE - 1) | (z * CHUNK_SIZE) | (y * CHUNK_SIZE * CHUNK_SIZE)];

	if (x == CHUNK_SIZE && right != nullptr)
		return right->data[0 | (z * CHUNK_SIZE) | (y * CHUNK_SIZE * CHUNK_SIZE)];

	if (z == -1 && back != nullptr)
		return back->data[x | ((CHUNK_SIZE - 1) * CHUNK_SIZE) | (y * CHUNK_SIZE * CHUNK_SIZE)];

	if (z == CHUNK_SIZE && front != nullptr)
		return front->data[x | (0 * CHUNK_SIZE) | (y * CHUNK_SIZE * CHUNK_SIZE)];

	// Return block from the current chunk if within bounds
	if (x >= 0 && x < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE)
		return data[x | (z * CHUNK_SIZE) | (y * CHUNK_SIZE * CHUNK_SIZE)];

	return AIR_BLOCK;
}


void Chunk::setBlockAt(BlockData value, unsigned x, unsigned y, unsigned z)
{
	if ((x >= CHUNK_SIZE) || (y >= CHUNK_SIZE_VERTICAL) || (z >= CHUNK_SIZE))
	{
		Log_warn << "Tried to set block at invalid coordinates. X: " << x << "Y: " << y << "Z: " << z << "\n";
		return;
	}
	data[x | (z * CHUNK_SIZE) | (y * CHUNK_SIZE * CHUNK_SIZE)] = value;
	dirty = true;
}

void Chunk::generateMesh()
{
	const glm::vec3 offsets[] = {
		{0, 0, -1},  // back face
		{0, 0, 1},   // front face
		{-1, 0, 0},  // left face
		{1, 0, 0},   // right face
		{0, -1, 0},  // bottom face
		{0, 1, 0}    // top face
	};

	// Abs value of relative positions of blocks used to calculate ambient oclusion for each vertex
	const glm::vec3 sideOffsets[3][2] = {
		{{1,0,0}, {0,1,0}},
		{{0, 1,0}, {0,0,1}},
		{{1,0,0}, {0,0,1}}
	};

	mesh.vertices.clear();
	mesh.vertices.reserve((CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE_VERTICAL) / 2 * 36);

	waterMesh.vertices.clear();
	waterMesh.vertices.reserve((CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE_VERTICAL) / 4 * 36);

	for (int x = 0; x < CHUNK_SIZE; ++x)
	{
		for (int z = 0; z < CHUNK_SIZE; ++z)
		{
			for (int y = 0; y < CHUNK_SIZE_VERTICAL; ++y)
			{
				BlockData currentBlock = getBlockAt(x, y, z);
				if (!currentBlock) continue;  // Skip empty blocks

				bool isWaterBlock = (currentBlock == WATER_BLOCK);
				Mesh& targetMesh = isWaterBlock ? waterMesh : mesh;

				for (unsigned face = 0; face < 6; ++face)
				{
					glm::vec3 neighborPos = glm::vec3(x, y, z) + offsets[face];
					BlockData neighborBlock = getBlockAt(neighborPos.x, neighborPos.y, neighborPos.z);

					// Skip the face if a neighboring block exists. 
					// For water blocks, skip the face if any neighboring block is present.
					// For non-water blocks, skip the face if the neighboring block is not water.
					if (neighborBlock && (isWaterBlock || neighborBlock != WATER_BLOCK))
						continue;

					glm::vec3 textureID = getBlockTextureID(currentBlock);
					for (unsigned vertex = 0; vertex < 6; ++vertex)
					{
						PackedVertexData newVertex = { 0 };
						VertexData vertexData = cube[face * 6 + vertex];

						setPosition(newVertex, (vertexData.position + glm::vec3(x, y, z)));

						// Assign textureID based on the face direction
						newVertex.textureID = offsets[face].x != 0 ? textureID.x :
							offsets[face].y != 0 ? textureID.y :
							textureID.z;

						glm::vec2 uv = vertexData.texCoords;
						newVertex.uv = (uv == glm::vec2(1, 1)) ? 3 :
							(uv == glm::vec2(1, 0)) ? 2 :
							(uv == glm::vec2(0, 1)) ? 1 : 0;

						if (!isWaterBlock)
						{
							// Calculate ambient oclusion if not a water block 
							// https://0fps.net/2013/07/03/ambient-occlusion-for-minecraft-like-worlds/

							glm::vec3 sideOffset1 = sideOffsets[face / 2][0];
							glm::vec3 sideOffset2 = sideOffsets[face / 2][1];
							glm::vec3 multiplier = glm::vec3(-1);
							if (vertexData.position.x == 1) multiplier.x = 1;
							if (vertexData.position.y == 1) multiplier.y = 1;
							if (vertexData.position.z == 1) multiplier.z = 1;

							glm::vec3 side1 = sideOffset1 * multiplier + glm::vec3(x, y, z) + offsets[face];
							glm::vec3 side2 = sideOffset2 * multiplier + glm::vec3(x, y, z) + offsets[face];
							glm::vec3 corner = (sideOffset1 + sideOffset2) * multiplier + glm::vec3(x, y, z) + offsets[face];

							bool blockSide1 = (getBlockAt(side1.x, side1.y, side1.z) && getBlockAt(side1.x, side1.y, side1.z) != WATER_BLOCK);
							bool blockSide2 = (getBlockAt(side2.x, side2.y, side2.z) && getBlockAt(side2.x, side2.y, side2.z) != WATER_BLOCK);
							bool blockCorner = (getBlockAt(corner.x, corner.y, corner.z) && getBlockAt(corner.x, corner.y, corner.z) != WATER_BLOCK);

							setAO(newVertex, calculateAO(blockSide1, blockSide2, blockCorner));
						}
						else
						{
							setAO(newVertex, 3.f); // Flat AO for water blocks
						}

						setNormal(newVertex, vertexData.normal);
						targetMesh.vertices.push_back(newVertex);
					}
				}
			}
		}
	}

	mesh.setup();
	waterMesh.setup();
	dirty = false;
}

#pragma endregion