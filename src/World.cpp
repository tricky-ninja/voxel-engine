#include "gameData.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Logger.h"
#include <GLFW/glfw3.h>
#include <algorithm>

#pragma region RENDERING

void World::render(const Shader& terrainShader, const Shader& waterShader, const Camera& camera)
{
	std::tuple<int, int> cameraPos = { camera.pos.x, camera.pos.z };

	terrainShader.bind();
	terrainShader.setFloat("renderDistance", RENDER_DISTANCE);
	terrainShader.setMat4("view", glm::value_ptr(camera.getView()));

	waterShader.bind();
	waterShader.setFloat("renderDistance", RENDER_DISTANCE);
	waterShader.setMat4("view", glm::value_ptr(camera.getView()));

	// Render solid stuff from closest to farthest from the player
	for (auto& key : sortedChunkIndicies)
	{
		int x = std::get<0>(key);
		int z = std::get<1>(key);

		Chunk* chunk = getChunk(x, z);
		if (chunk == nullptr) continue;
		glm::vec3 pos = { x * CHUNK_SIZE, 0, z * CHUNK_SIZE };

		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, pos);

		drawMesh(chunk->mesh, terrainShader, camera, model);
	}

	// Render water(transparent stuff) from farthest to closest to the player
	for (auto it = sortedChunkIndicies.rbegin(); it != sortedChunkIndicies.rend(); ++it)
	{
		auto& key = *it;
		int x = std::get<0>(key);
		int z = std::get<1>(key);

		Chunk* chunk = getChunk(x, z);
		if (chunk == nullptr) continue;
		glm::vec3 pos = { x * CHUNK_SIZE, 0, z * CHUNK_SIZE };

		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, pos);
		waterShader.bind();
		waterShader.setFloat("time", (float)glfwGetTime());
		glDisable(GL_CULL_FACE);
		if (chunk->waterMesh.vertices.size() > 0) drawMesh(chunk->waterMesh, waterShader, camera, model);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
	}
}

#pragma endregion

#pragma region CHUNK_MANAGEMENT

Chunk* World::getChunk(int x, int z)
{
	auto it = chunks.find({ x, z });
	if (it == chunks.end()) return nullptr;
	return it->second;

}

// TODO: Implement cubic chunks
void World::generateChunkData(int chunkX, int chunkZ)
{
	// Check if the chunk already exists, and return if it does
	if (chunks.find({ chunkX, chunkZ }) != chunks.end()) return;

	Chunk* chunk = new Chunk();
	if (chunk == nullptr) return;
	// Add a new chunk to the world and mark it as dirty
	chunks.emplace(std::make_tuple(chunkX, chunkZ), chunk);
	sortedChunkIndicies.push_back({ chunkX, chunkZ });



	// Iterate through each block in the chunk
	for (int x = 0; x < CHUNK_SIZE; ++x)
	{
		for (int z = 0; z < CHUNK_SIZE; ++z)
		{
			// Calculate the height using a sine-based terrain generation algorithm
			int height = std::max<float>(
				(sin((x + chunkX * CHUNK_SIZE) / 40.0f) * 50.0f +
					sin((z + chunkZ * CHUNK_SIZE) / 50.0f) * 60.0f +
					sin((x + chunkX * CHUNK_SIZE) / 3.0f) * 4.0f +
					sin((z + chunkZ * CHUNK_SIZE) / 3.0f) * 4.0f +
					sin((x + chunkX * CHUNK_SIZE + 201) / 16.0f) * 5.0f * cos((z + chunkZ * CHUNK_SIZE + 420) / 12.0f) * 5.0f +
					sin((x + chunkX * CHUNK_SIZE + 469) / 8.0f) * 2.0f * cos((z + chunkZ * CHUNK_SIZE + 690) / 8.0f) * 2.0f),
				-32.0f
			) + 64.0f; // Add an offset to the height

			for (int y = 0; y < CHUNK_SIZE_VERTICAL; ++y)
			{
				// Set block type based on height and layer
				if (y < height) // Below the terrain surface
				{
					unsigned int stoneHeight = rand() % 10;
					if (y > 84 + stoneHeight)
						chunk->setBlockAt(DIRT_BLOCK, x, y, z);
					else
						chunk->setBlockAt(STONE_BLOCK, x, y, z);

					unsigned sandHeight = rand() % 3;
					if (y >= 63 && y < 64 + 6 + sandHeight)
						chunk->setBlockAt(SAND_BLOCK, x, y, z);
				}
				else if (y < 64) // Underwater (below sea level)
				{
					chunk->setBlockAt(WATER_BLOCK, x, y, z);
				}
				else // Above terrain surface
				{
					chunk->setBlockAt(AIR_BLOCK, x, y, z);

					// If the block below is dirt, set the appropriate surface block
					if (chunk->getBlockAt(x, y - 1, z) == DIRT_BLOCK)
					{
						if (y - 1 > 160)
							chunk->setBlockAt(SNOW_BLOCK, x, y - 1, z);
						else
							chunk->setBlockAt(GRASS_BLOCK, x, y - 1, z);
					}
				}
			}
		}
	}

	// Mark the chunk as dirty for future updates
	chunk->dirty = true;
}

#pragma endregion

#pragma region TERRAIN_LOADING_STUFF

void World::applyUpdates()
{
	// Generate the chunk data for loaded chunks
	// Cap the max amounts of chunks that can be generated per frame
	unsigned i = 0;
	while ((!chunkQueue.empty()) && i < maxChunksPerFrame)
	{
		std::tuple<int, int> chunkIndex = chunkQueue.front();
		int x = std::get<0>(chunkIndex);
		int z = std::get<1>(chunkIndex);

		auto it = std::find(chunksToDelete.begin(), chunksToDelete.end(), chunkIndex);

		if (it != chunksToDelete.end()) {
			chunkQueue.pop_front();
			continue;
		}

		Log_debug << "Generated: " << x << ", " << z << "\n";
		generateChunkData(x, z);
		i++;
		chunkQueue.pop_front();
	}

	// Check if any of the generated chunks need to update their mesh
	for (auto& chunk : chunks)
	{
		int x = std::get<0>(chunk.first);
		int z = std::get<1>(chunk.first);

		Chunk* chunkObj = chunk.second;


		// Flag will be zero if the newChunk and neighbor are equal, otherwise it will be non zero
		auto updateFlag = [](Chunk*& neighbor, int& flags, int bit, Chunk* newChunk)
		{
			flags |= (neighbor == nullptr) << bit;  // Store current state
			neighbor = newChunk;                    // Update neighbor
			flags ^= (neighbor == nullptr) << bit;  // XOR to detect changes
		};


		int changeFlags = 0;


		// If any chunk neighbouring the current chunk is changed then regenerate the current chunk's mesh
		updateFlag(chunkObj->left, changeFlags, 0, getChunk(x - 1, z));
		updateFlag(chunkObj->right, changeFlags, 1, getChunk(x + 1, z));
		updateFlag(chunkObj->front, changeFlags, 2, getChunk(x, z + 1));
		updateFlag(chunkObj->back, changeFlags, 3, getChunk(x, z - 1));

		if (changeFlags != 0) {
			chunkObj->dirty = true;
		}

		if (chunkObj->dirty) chunkObj->generateMesh();
	}

	// Delete chunks scheduled for deletion
	for (const auto& key : chunksToDelete)
	{
		int x = std::get<0>(key);
		int z = std::get<1>(key);

		Chunk* chunkToBeDeleted = chunks[key];
		if (chunkToBeDeleted == nullptr) { 
			chunks.erase(key);
			sortedChunkIndicies.erase(std::remove(sortedChunkIndicies.begin(), sortedChunkIndicies.end(), key), sortedChunkIndicies.end());
			Log_debug << "Erased " << x << ", " << z << "\n";
			continue; 
		}

		Chunk* left = chunkToBeDeleted->left;
		Chunk* right = chunkToBeDeleted->right;
		Chunk* front = chunkToBeDeleted->front;
		Chunk* back = chunkToBeDeleted->back;

		// Remove all references of this chunk from its neigbours
		if (left != nullptr) left->right = nullptr;
		if (right != nullptr) right->left = nullptr;
		if (front != nullptr) front->back = nullptr;
		if (back != nullptr) back->front = nullptr;

		delete chunkToBeDeleted;
		chunks.erase(key);
		sortedChunkIndicies.erase(std::remove(sortedChunkIndicies.begin(), sortedChunkIndicies.end(), key), sortedChunkIndicies.end());
		Log_debug << "Erased " << x << ", " << z << "\n";
	}
	chunksToDelete.clear();
}

// TODO: Implement cubic chunks
void World::updateState(glm::vec3 currentPos)
{
	int startX = currentPos.x / CHUNK_SIZE;
	int startZ = currentPos.z / CHUNK_SIZE;

	// If player hasn't moved between chunks
	if (startX == lastX && startZ == lastZ)
	{
		if (!firstLoad) return;

		// If this is the first time this function is called
		sortedChunkIndicies.reserve((2 * RENDER_DISTANCE) * (2 * RENDER_DISTANCE) * 2);
		firstLoad = false;
	}

	lastX = startX;
	lastZ = startZ;

	// Schedule the chunks to be generated in a spiral order from the player
	for (int layer = 0; layer < RENDER_DISTANCE; layer++)
	{
		for (int z = -layer; z < layer; z++)
		{
			for (int x = -layer; x < layer; x++)
			{
				if (chunks.find({ x + startX, z + startZ }) != chunks.end()) continue;
				chunkQueue.push_back({ x + startX, z + startZ });
			}
		}
	}


	// Schedule chunks outside render distance to be deleted
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

	for (const auto& chunkIndex : chunkQueue)
	{
		int x = std::get<0>(chunkIndex);
		int z = std::get<1>(chunkIndex);

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
}

void World::deleteAll()
{
	chunksToDelete.clear();

	for (const auto& chunk : chunks)
	{
		int x = std::get<0>(chunk.first);
		int z = std::get<1>(chunk.first);
		chunksToDelete.push_back(std::make_tuple(x, z));
	}

	for (const auto& key : chunksToDelete)
	{
		int x = std::get<0>(key);
		int z = std::get<1>(key);

		Chunk* chunkToBeDeleted = chunks[key];

		delete chunkToBeDeleted;
		chunks.erase(key);
		sortedChunkIndicies.erase(std::remove(sortedChunkIndicies.begin(), sortedChunkIndicies.end(), key), sortedChunkIndicies.end());
	}
}

#pragma endregion


