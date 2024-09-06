#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	GLuint VAO, VBO, EBO;

	void setup();
};

struct Shader
{
	GLuint id;

	bool loadFromFile(const char* vertexShaderPath, const char* fragmentShaderPath);

	bool setBool(const std::string& name, bool value) const;
	bool setInt(const std::string& name, int value) const;
	bool setUnsigned(const std::string& name, unsigned value) const;
	bool setFloat(const std::string& name, float value) const;

	bool setVec2(const std::string& name, const float* values) const;
	bool setVec3(const std::string& name, const float* values) const;
	bool setVec4(const std::string& name, const float* values) const;

	bool setMat2(const std::string& name, const float* value) const;
	bool setMat3(const std::string& name, const float* value) const;
	bool setMat4(const std::string& name, const float* value) const;

	void bind() const;
	void unbind() const;
	void clear();
};

struct Texture
{
	unsigned int id = 0;

	int slot;

	int width;
	int height;
	int bitsPerPixel;

	bool loadFromFile(const char* filepath, unsigned int wrap = GL_REPEAT, unsigned int filtering = GL_NEAREST);
	void bind(int slot);
	void unbind();

	void clear();
};

const float YAW = -90.f;
const float PITCH = 0.0f;
const float SPEED = 6;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

struct Camera
{
	glm::vec3 pos;
	glm::vec3 direction;
	glm::vec3 up;

	bool isActive = false;

	float yaw = YAW;
	float pitch = PITCH;

	float moveSpeed = SPEED;
	float sensitivity = SENSITIVITY;
	float fov = ZOOM;

	void move(Camera_Movement moveDirection, float deltaTime);
	void turn(float xoffset, float yoffset, bool constraintPitch = true);
	void zoom(float yoffset);

	void lookAt(glm::vec3 target);
	glm::mat4 getView() const;

	void updateVectors();
	std::string getCoordsAsString();
};

void drawMesh(const Mesh& mesh, const Shader &shader, const Camera& camera);