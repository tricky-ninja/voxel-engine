#include "gameData.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Logger.h"

#include "Cube.h"

#pragma region STATIC_FUNCTIONS

static int calculate_AO(bool side1, bool side2, bool corner) {
	if (side1 && side2) return 0;
	return 3 - (side1 + side2 + corner);
}

static glm::vec3 get_block_textureID(BlockType block)
{
	glm::vec3 blockTextures[BLOCK_TYPE_COUNT - 1] = {
		glm::vec3(3,0,3), // GRASS_BLOCK
		glm::vec3(2,2,2), // DIRT_BLOCK
		glm::vec3(223,223,223),  // WATER_BLOCK
		glm::vec3(18,18,18),  // SAND_BLOCK
		glm::vec3(1,1,1),  // STONE_BLOCK
		glm::vec3(66,66,66),  // SNOW_BLOCK
		glm::vec3(12,12,12),  // RED_FLOWER
		glm::vec3(13,13,13),  // YELLOW_FLOWER
	};

	if (block > 0 && block < BLOCK_TYPE_COUNT)
	{
		return blockTextures[block - 1];
	}
	return glm::vec3(14, 14, 14);	// return empty texture(pink color) if block id is invalid
}

static BlockCategory get_block_category(BlockType block)
{
	if (block == WATER_BLOCK) return WATER;
	if (block == RED_FLOWER || block == YELLOW_FLOWER) return TRANSPARENT;
	return SOLID;
}

static MeshType get_block_mesh(BlockType block)
{
	if (block == RED_FLOWER || block == YELLOW_FLOWER) return CROSS_MESH;
	return CUBE_MESH;
}

#pragma endregion

#pragma region CHUNK_STUFF

BlockData Chunk::get_block_at(int x, unsigned y, int z)
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


void Chunk::set_block(BlockData value, unsigned x, unsigned y, unsigned z)
{
	if ((x >= CHUNK_SIZE) || (y >= CHUNK_SIZE_VERTICAL) || (z >= CHUNK_SIZE))
	{
		Log_warn << "Tried to set block at invalid coordinates. X: " << x << "Y: " << y << "Z: " << z << "\n";
		return;
	}
	data[x | (z * CHUNK_SIZE) | (y * CHUNK_SIZE * CHUNK_SIZE)] = value;
	dirty = true;
}

void Chunk::generate_mesh()
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
	waterMesh.vertices.clear();
	transparentMesh.vertices.clear();

	for (int x = 0; x < CHUNK_SIZE; ++x)
	{
		for (int z = 0; z < CHUNK_SIZE; ++z)
		{
			for (int y = 0; y < CHUNK_SIZE_VERTICAL; ++y)
			{
				BlockData currentBlock = get_block_at(x, y, z);
				if (!currentBlock) continue;  // Skip empty blocks

				bool isWaterBlock = (currentBlock == WATER_BLOCK);
				Mesh& targetMesh = isWaterBlock ? waterMesh : mesh;
				BlockCategory blockType = get_block_category(currentBlock);
				MeshType blockMeshType = get_block_mesh(currentBlock);

				if (blockMeshType == CROSS_MESH)
				{
					PackedVertexData newVertex1 = { 0 };
					PackedVertexData newVertex2 = { 0 };
					PackedVertexData newVertex3 = { 0 };
					PackedVertexData newVertex4 = { 0 };

					PackedVertexData newVertex5 = { 0 };
					PackedVertexData newVertex6 = { 0 };
					PackedVertexData newVertex7 = { 0 };
					PackedVertexData newVertex8 = { 0 };

					VertexData vertex1 = cube[0];
					VertexData vertex2 = cube[6 + 1];
					VertexData vertex3 = cube[1 ];
					VertexData vertex4 = cube[6+2];

					VertexData vertex5 = cube[2];
					VertexData vertex6 = cube[6 + 4];
					VertexData vertex7 = cube[2+2];
					VertexData vertex8 = cube[6 + 0];

					set_position(newVertex1, (vertex1.position) + glm::vec3(x, y, z));
					set_position(newVertex2, (vertex2.position) + glm::vec3(x, y, z));
					set_position(newVertex3, (vertex3.position) + glm::vec3(x, y, z));
					set_position(newVertex4, (vertex4.position) + glm::vec3(x, y, z));

					set_position(newVertex5, (vertex5.position) + glm::vec3(x, y, z));
					set_position(newVertex6, (vertex6.position) + glm::vec3(x, y, z));
					set_position(newVertex7, (vertex7.position) + glm::vec3(x, y, z));
					set_position(newVertex8, (vertex8.position) + glm::vec3(x, y, z));

					newVertex1.textureID = get_block_textureID(currentBlock).x;
					newVertex2.textureID = get_block_textureID(currentBlock).x;
					newVertex3.textureID = get_block_textureID(currentBlock).x;
					newVertex4.textureID = get_block_textureID(currentBlock).x;

					newVertex5.textureID = get_block_textureID(currentBlock).x;
					newVertex6.textureID = get_block_textureID(currentBlock).x;
					newVertex7.textureID = get_block_textureID(currentBlock).x;
					newVertex8.textureID = get_block_textureID(currentBlock).x;

					newVertex1.uv = 0;
					newVertex2.uv = 2;
					newVertex3.uv = 1;
					newVertex4.uv = 3;

					newVertex5.uv = 1;
					newVertex6.uv = 3;
					newVertex7.uv = 0;
					newVertex8.uv = 2;

					set_AO(newVertex1, 3);
					set_AO(newVertex2, 3);
					set_AO(newVertex3, 3);
					set_AO(newVertex4, 3);

					set_AO(newVertex5, 3);
					set_AO(newVertex6, 3);
					set_AO(newVertex7, 3);
					set_AO(newVertex8, 3);

					transparentMesh.vertices.push_back(newVertex1);
					transparentMesh.vertices.push_back(newVertex2);
					transparentMesh.vertices.push_back(newVertex3);
					transparentMesh.vertices.push_back(newVertex2);
					transparentMesh.vertices.push_back(newVertex3);
					transparentMesh.vertices.push_back(newVertex4);

					transparentMesh.vertices.push_back(newVertex5);
					transparentMesh.vertices.push_back(newVertex6);
					transparentMesh.vertices.push_back(newVertex7);
					transparentMesh.vertices.push_back(newVertex6);
					transparentMesh.vertices.push_back(newVertex7);
					transparentMesh.vertices.push_back(newVertex8);

				}

				else
				{


					for (unsigned face = 0; face < 6; ++face)
					{
						glm::vec3 neighborPos = glm::vec3(x, y, z) + offsets[face];
						BlockData neighborBlock = get_block_at(neighborPos.x, neighborPos.y, neighborPos.z);
						BlockCategory neighborBlockType = get_block_category(neighborBlock);

						// Skip the face if a neighboring block exists. 
						// For non solid blocks, skip the face if any neighboring block is present.
						// For solid blocks, skip the face if the neighboring block is solid.
						if (neighborBlock && ((blockType != SOLID) || neighborBlockType == SOLID))
							continue;

						glm::vec3 textureID = get_block_textureID(currentBlock);
						for (unsigned vertex = 0; vertex < 6; ++vertex)
						{
							PackedVertexData newVertex = { 0 };
							VertexData vertexData = cube[face * 6 + vertex];

							set_position(newVertex, (vertexData.position + glm::vec3(x, y, z)));

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

								bool blockSide1 = (get_block_at(side1.x, side1.y, side1.z) && get_block_category(get_block_at(side1.x, side1.y, side1.z)) == SOLID);
								bool blockSide2 = (get_block_at(side2.x, side2.y, side2.z) && get_block_category(get_block_at(side2.x, side2.y, side2.z)) == SOLID);
								bool blockCorner = (get_block_at(corner.x, corner.y, corner.z) && get_block_category(get_block_at(corner.x, corner.y, corner.z)) == SOLID);

								set_AO(newVertex, calculate_AO(blockSide1, blockSide2, blockCorner));
							}
							else
							{
								set_AO(newVertex, 3.f); // Flat AO for water blocks
							}

							set_normal(newVertex, vertexData.normal);
							targetMesh.vertices.push_back(newVertex);
						}
					}
				}
			}
		}
	}

	mesh.setup();
	waterMesh.setup();
	transparentMesh.setup();
	dirty = false;
}

#pragma endregion