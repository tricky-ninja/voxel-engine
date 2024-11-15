#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

enum FACE
{
	FACE_LEFT = 0b0,
	FACE_RIGHT = 0b1,
	FACE_FRONT = 0b10,
	FACE_BACK = 0b11,
	FACE_TOP = 0b100,
	FACE_BOTTOM = 0b101,
};

struct PackedVertexData
{
	char x, y, z;
	char face;
	char ao;
	char uv;
	char textureID;
};



void set_position(PackedVertexData &vertexData, glm::vec3 position);
glm::vec3 get_position(PackedVertexData &vertexData);

void set_normal(PackedVertexData &vertexData, glm::vec3 normal);
glm::vec3 get_normal(PackedVertexData &vertexData);

void set_AO(PackedVertexData &vertexData, uint8_t ao);
uint8_t get_AO(PackedVertexData &vertexData);


struct Mesh
{
	std::vector<PackedVertexData> vertices;
	GLuint VAO, VBO, EBO;

	void setup();
};

struct Shader
{
	GLuint id;

	std::string vertexFilepath;
	std::string fragmentFilepath;
	long long vertexTimestamp;
	long long fragmentTimestamp;

	bool load_from_file(const char* vertexShaderPath, const char* fragmentShaderPath);

	bool set_bool(const std::string& name, bool value) const;
	bool set_int(const std::string& name, int value) const;
	bool set_unsigned(const std::string& name, unsigned value) const;
	bool set_float(const std::string& name, float value) const;

	bool set_vec2(const std::string& name, const float* values) const;
	bool set_vec3(const std::string& name, const float* values) const;
	bool set_vec4(const std::string& name, const float* values) const;

	bool set_mat2(const std::string& name, const float* value) const;
	bool set_mat3(const std::string& name, const float* value) const;
	bool set_mat4(const std::string& name, const float* value) const;

	void hot_reload();

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
	std::string filepath;
	long long timestamp;

	bool load_from_file(const char* filepath, unsigned int wrap = GL_REPEAT, unsigned int filtering = GL_NEAREST);
	void hot_reload();
	void bind(int slot);
	void unbind();

	void clear();
};

struct Framebuffer
{
	unsigned int id = 0;
	unsigned int textureColorBuffer = 0;
	unsigned int depthStencilBuffer = 0;

	Framebuffer() {};
	Framebuffer(unsigned width, unsigned height);
	
	void bind();
	void unbind();
	void clear();
};

const float YAW = -90.f;
const float PITCH = 0.0f;
const float SPEED = 16;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

enum CAMERA_MOVEMENT {
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

	void move(CAMERA_MOVEMENT moveDirection, float deltaTime);
	void turn(float xoffset, float yoffset, bool constraintPitch = true);
	void zoom(float yoffset);

	void look_at(glm::vec3 target);
	glm::mat4 get_view() const;

	void update_vectors();
	std::string get_coords_as_string();
};

void draw_mesh(const Mesh& mesh, const Shader &shader, const Camera& camera, glm::mat4 model);