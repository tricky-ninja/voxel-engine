#include "gameData.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Logger.h"

#pragma region HELPER_VARS

static struct VertexData
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

static std::vector<VertexData> cube = {
	// Back face
	{ glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
	{ glm::vec3(0.f,  1.f, 0.f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f) },
	{ glm::vec3(1.f,  1.f, 0.f),  glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
	{ glm::vec3(1.f,  1.f, 0.f),  glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
	{ glm::vec3(1.f, 0.f, 0.f),  glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
	{ glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },

	// Front face
	{ glm::vec3(0.f, 0.f,  1.f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
	{ glm::vec3(1.f, 0.f,  1.f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f) },
	{ glm::vec3(1.f,  1.f,  1.f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
	{ glm::vec3(1.f,  1.f,  1.f),  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
	{ glm::vec3(0.f,  1.f,  1.f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f) },
	{ glm::vec3(0.f, 0.f,  1.f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },

	// Left face
	{ glm::vec3(0.f, 0.f, 0.f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
	{ glm::vec3(0.f, 0.f,  1.f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
	{ glm::vec3(0.f,  1.f,  1.f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
	{ glm::vec3(0.f,  1.f,  1.f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
	{ glm::vec3(0.f,  1.f, 0.f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
	{ glm::vec3(0.f, 0.f, 0.f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

	// Right face
	{ glm::vec3(1.f, 0.f, 0.f),  glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
	{ glm::vec3(1.f,  1.f, 0.f),  glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
	{ glm::vec3(1.f,  1.f,  1.f),  glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
	{ glm::vec3(1.f,  1.f,  1.f),  glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
	{ glm::vec3(1.f, 0.f,  1.f),  glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
	{ glm::vec3(1.f, 0.f, 0.f),  glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

	// Bottom face
	{ glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
	{ glm::vec3(1.f, 0.f, 0.f),  glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
	{ glm::vec3(1.f, 0.f,  1.f),  glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
	{ glm::vec3(1.f, 0.f,  1.f),  glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
	{ glm::vec3(0.f, 0.f,  1.f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
	{ glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

	// Top face
	{ glm::vec3(0.f,  1.f, 0.f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
	{ glm::vec3(0.f,  1.f,  1.f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
	{ glm::vec3(1.f,  1.f,  1.f),  glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
	{ glm::vec3(1.f,  1.f,  1.f),  glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
	{ glm::vec3(1.f,  1.f, 0.f),  glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
	{ glm::vec3(0.f,  1.f, 0.f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f) }
};

#pragma endregion

int calculateAO(bool side1, bool side2, bool corner) {
	if (side1 && side2) return 0;
	return 3 - (side1 + side2 + corner);
}

BlockData Chunk::blockAt(int x, unsigned y, int z)
{
	// Boundary checks
	if (x < -1 || x > CHUNK_SIZE || z < -1 || z > CHUNK_SIZE || y >= CHUNK_SIZE_VERTICAL)
		return false;

	// Special corner cases: reject invalid boundary conditions
	if ((x == -1 && z == -1) || (x == -1 && z == CHUNK_SIZE) ||
		(x == CHUNK_SIZE && z == -1) || (x == CHUNK_SIZE && z == CHUNK_SIZE))
		return false;

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

	return false;
}


void Chunk::setBlock(BlockData value, unsigned x, unsigned y, unsigned z)
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

	const glm::vec3 sideOffsets[3][2] = {
		{{1,0,0}, {0,1,0}},
		{{0, 1,0}, {0,0,1}},
		{{1,0,0}, {0,0,1}}
	};
	mesh.vertices.clear();
	mesh.vertices.reserve((CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE_VERTICAL)/2 * 36);
	for (int x = 0; x < CHUNK_SIZE; ++x)
	{
		for (int z = 0; z < CHUNK_SIZE; ++z)
		{
			for (int y = 0; y < CHUNK_SIZE_VERTICAL; ++y)
			{
				if (!blockAt(x, y, z))
					continue;  // Skip empty blocks

				for (unsigned face = 0; face < 6; ++face)
				{
					glm::vec3 neighborPos = glm::vec3(x, y, z) + offsets[face];

					// Skip faces where the neighbor block exists
					if (blockAt(neighborPos.x, neighborPos.y, neighborPos.z))
						continue;

					glm::vec3 sideOffset1 = sideOffsets[face / 2][0];
					glm::vec3 sideOffset2 = sideOffsets[face / 2][1];
					
					for (unsigned vertex = 0; vertex < 6; ++vertex)
					{
						PackedVertexData newVertex = { 0 };
						setPosition(newVertex, (cube[face * 6 + vertex].position + glm::vec3(x, y, z)));

						glm::vec3 multiplier = glm::vec3(-1);
						if (cube[face * 6 + vertex].position.x == 1) multiplier.x = 1;
						if (cube[face * 6 + vertex].position.y == 1) multiplier.y = 1;
						if (cube[face * 6 + vertex].position.z == 1) multiplier.z = 1;
						glm::vec3 side1 = (sideOffset1 * multiplier) + glm::vec3(x, y, z) + offsets[face];
						glm::vec3 side2 = (sideOffset2 * multiplier) + glm::vec3(x, y, z) + offsets[face];
						glm::vec3 corner = ((sideOffset2 + sideOffset1) * multiplier) + glm::vec3(x, y, z) + offsets[face];
						
						glm::vec2 uv;

						if (offsets[face].x != 0)
						{
							newVertex.textureID = 3;
						}
						else if (offsets[face].y != 0)
						{
							newVertex.textureID = 0;
						}
						else if (offsets[face].z != 0)
						{
							newVertex.textureID = 3;
						}
						uv = cube[face * 6 + vertex].texCoords;
						
						if (uv == glm::vec2(1, 1)) newVertex.uv = 3;
						if (uv == glm::vec2(1, 0)) newVertex.uv = 2;
						if (uv == glm::vec2(0, 1)) newVertex.uv = 1;
						if (uv == glm::vec2(0, 0)) newVertex.uv = 0;


						bool blockSide1 = blockAt(side1.x, side1.y, side1.z);
						bool blockSide2 = blockAt(side2.x, side2.y, side2.z);
						bool blockCorner = blockAt(corner.x, corner.y, corner.z);
						setAO(newVertex,calculateAO(blockSide1, blockSide2, blockCorner));
						//setAO(newVertex, 3.f);

						setNormal(newVertex, cube[face * 6 + vertex].normal);
						mesh.vertices.push_back(newVertex);
					}
				}
			}
		}
	}
	mesh.setup();
	dirty = false;
}

Chunk* ChunkManager::getChunk(int x, int z)
{
	if (chunks.find({x,z}) == chunks.end()) return nullptr;
	return &chunks.find({ x,z })->second;
}

void ChunkManager::render(const Shader& shader, const Camera &camera)
{
	for (const auto& chunk : chunks)
	{
		float x = std::get<0>(chunk.first);
		float z = std::get<1>(chunk.first);

		glm::vec3 pos = { x * CHUNK_SIZE, 0, z * CHUNK_SIZE};

		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, pos);
		shader.bind();
		shader.setFloat("renderDistance", RENDER_DISTANCE);
		shader.unbind();
		drawMesh(chunk.second.mesh, shader, camera, model);
	}
}

// TODO: Implement cubic chunks
void ChunkManager::generate(int chunkX, int chunkZ)
{
	if (chunks.find({ chunkX,chunkZ }) != chunks.end()) return;
	Log_debug << chunkX << ", " << chunkZ << "\n";
	
	chunks.emplace(std::make_tuple(chunkX, chunkZ), Chunk());
	Chunk &chunk = chunks.find({chunkX, chunkZ})->second;
	
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			unsigned height = std::max<float>(abs((sin((x + chunkX * CHUNK_SIZE + 0) / 32.0f) * 10.0f + sin((z + chunkZ * CHUNK_SIZE + 0) / 40.0f) * 10.0f)
				+ (sin((x + chunkX * CHUNK_SIZE + (0 + 0) % 100) / 16.0f) * 5.0f * cos((z + chunkZ * CHUNK_SIZE + 0) / 20.0f) * 5.0f)
				+ (sin((x + chunkX * CHUNK_SIZE + (0 + 0) % 100) / 8.0f) * 2.0f * cos((z + chunkZ * CHUNK_SIZE + 0) / 8.0f) * 2.0f)), 1);
			height += 64.0f;

			for (int y = 0; y < CHUNK_SIZE_VERTICAL; y++)
			{
				if (y < height) chunk.setBlock(true, x, y, z);
				else chunk.setBlock(false, x, y, z);
			}
		}
	}
	chunk.dirty = true;
}

void ChunkManager::update(glm::vec3 currentPos)
{
	int startX = currentPos.x / CHUNK_SIZE;
	int startZ = currentPos.z / CHUNK_SIZE;

	if (startX == lastX && startZ == lastZ)
	{
		if (!firstLoad) return;
		firstLoad = false;
	}

	lastX = startX;
	lastZ = startZ;

	for (int x = -RENDER_DISTANCE+1; x < RENDER_DISTANCE; x++)
	{
		for (int z = -RENDER_DISTANCE+1; z < RENDER_DISTANCE; z++)
		{
			generate(x + startX, z + startZ);
		}
	}

	for (auto& chunk : chunks)
	{
		int x = std::get<0>(chunk.first);
		int z = std::get<1>(chunk.first);

		Chunk& chunkObj = chunk.second;

		auto updateFlag = [](Chunk*& neighbor, int& flags, int bit, Chunk* newChunk) {
			flags |= (neighbor == nullptr) << bit;  // Store current state
			neighbor = newChunk;                    // Update neighbor
			flags ^= (neighbor == nullptr) << bit;  // XOR to detect changes
			};

		int changeFlags = 0;

		updateFlag(chunkObj.left, changeFlags, 0, getChunk(x - 1, z));
		updateFlag(chunkObj.right, changeFlags, 1, getChunk(x + 1, z));
		updateFlag(chunkObj.front, changeFlags, 2, getChunk(x, z + 1));
		updateFlag(chunkObj.back, changeFlags, 3, getChunk(x, z - 1));

		if (changeFlags != 0) {
			chunkObj.dirty = true;
		}

		if (chunkObj.dirty) chunkObj.generateMesh();
	}

	std::vector<std::tuple<int, int>> chunksToDelete;
	for (const auto& chunk : chunks)
	{
		int x = std::get<0>(chunk.first);
		int z = std::get<1>(chunk.first);

		if (x <= (-RENDER_DISTANCE + startX) || x >= (RENDER_DISTANCE + startX))
		{
			Log_debug << "Pushed to erase: " << x << ", " << z << "\n";
			chunksToDelete.push_back(std::make_tuple(x, z));
		}
		else if (z <= (-RENDER_DISTANCE + startZ) || z >= (RENDER_DISTANCE + startZ))
		{
			Log_debug << "Pushed to erase: " << x << ", " << z << "\n";
			chunksToDelete.push_back(std::make_tuple(x, z));

		}
	}

	for (const auto& key : chunksToDelete)
	{
		//free(chunks[key]);
		chunks.erase(key);
	}

	
}
