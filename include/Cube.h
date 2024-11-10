#pragma once

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
