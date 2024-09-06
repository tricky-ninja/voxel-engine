#pragma once
#include "Renderer.h"

struct Chunk
{
	bool data[64 * 64 * 64];
	bool blockAt(unsigned x, unsigned y, unsigned z);
	void setBlock(bool value, unsigned x, unsigned y, unsigned z);

	Mesh generateMesh();
};