#include "game.h"
#include "Logger.h"
#include "random"
#include "gameData.h"
#include <algorithm>

#define NUM_CHUNK 30

struct GameContext
{
	Chunk chunks[NUM_CHUNK * NUM_CHUNK];
	Camera cam;
	Shader mainShader;
	GLFWwindow* window;
	Mesh chunkMesh[NUM_CHUNK * NUM_CHUNK];
	bool renderWrieframe = false;
};

GameContext context;

float lastX = 0;
float lastY = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_F3:
			context.renderWrieframe = !context.renderWrieframe;
			break;
		}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	if (context.cam.isActive) context.cam.turn(xoffset, yoffset);
}


void init(GLFWwindow* window)
{
	srand(20);
	context.cam;
	context.cam.pos = { 1.0f, 64.0f, 5.0f };
	context.cam.up = { 0,1,0 };
	context.cam.turn(1200, -400);
	context.cam.updateVectors();

	unsigned randVal = rand() % 100;
	unsigned randVal2 = rand() % 100;

	float totalTime = 0;
	for (int i = 0; i < NUM_CHUNK; i++)
	{
		for (int j = 0; j < NUM_CHUNK; j++)
		{

			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				for (int z = 0; z < CHUNK_SIZE; z++)
				{
					unsigned height = std::max<float>((sin((x + i * CHUNK_SIZE + randVal) / 32.0f) * 10.0f + sin((z  + j * CHUNK_SIZE+ randVal) / 40.0f) * 10.0f)    // Large rolling hills
						+ (sin((x +i * CHUNK_SIZE + (randVal + randVal2) % 100) / 16.0f) * 5.0f * cos((z + j * CHUNK_SIZE + randVal2) / 20.0f) * 5.0f)       // Smaller details
						+ (sin((x+ i * CHUNK_SIZE + (randVal + randVal2) % 100) / 8.0f) * 2.0f * cos((z + j * CHUNK_SIZE + randVal2) / 8.0f) * 2.0f),1);      // Fine noise-like bumps;
					height += 64.0f;
					//height = 256.f;
					//height = 16;
					for (int y = 0; y < CHUNK_SIZE_VERTICAL; y++)
					{
						if (y < height) context.chunks[i * NUM_CHUNK + j].setBlock(true, x, y, z);
						else context.chunks[i * NUM_CHUNK + j].setBlock(false, x, y, z);
						//context.chunks[i * NUM_CHUNK + j].setBlock(((x + y + z) % 2 == 0) ? 1 : 0, x, y, z);
					}
				}
			}
			context.chunks[i * NUM_CHUNK + j].position = { i,0,j };
			float time = glfwGetTime();
			context.chunks[i * NUM_CHUNK + j].generateMesh();
			float chunkTime = glfwGetTime() - time;
			chunkTime *= 1000;
			Log_debug << "Chunk created: " << chunkTime << "ms\n";
			totalTime += chunkTime;
		}
	}

	Log_debug << "Time taken for " << NUM_CHUNK * NUM_CHUNK << " chunks: " << totalTime << "ms\n";
	Log_debug << "Average time: " << totalTime / (NUM_CHUNK * NUM_CHUNK) << "ms\n";

	context.window = window;

	context.cam.isActive = false;

	permAssert_msg(context.mainShader.loadFromFile(ASSETS_PATH "shaders/main.vert", ASSETS_PATH "shaders/main.frag"), "Failed to load main shaders");
}

void update(float deltaTime)
{
	glClearColor(185 / 255.f, 233 / 255.f, 250 / 255.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (context.renderWrieframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLFWwindow* window = context.window;


	if (context.cam.isActive) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		context.cam.isActive = true;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		context.cam.isActive = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		context.cam.move(FORWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		context.cam.move(BACKWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		context.cam.move(LEFT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		context.cam.move(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		context.cam.move(UP, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		context.cam.move(DOWN, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		context.cam.moveSpeed = 20;

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
		context.cam.moveSpeed = 6;


	for (int i = 0; i < NUM_CHUNK; i++) {
		for (int j = 0; j < NUM_CHUNK; j++) {
			glm::mat4 model = glm::mat4(1);
			model = glm::translate(model, { context.chunks[i * NUM_CHUNK + j].position.x * CHUNK_SIZE, context.chunks[i * NUM_CHUNK + j].position.y, context.chunks[i * NUM_CHUNK + j].position.z * CHUNK_SIZE });
			
			/*float time = glfwGetTime();
			context.chunks[i * NUM_CHUNK + j].generateMesh();
			float chunkTime = glfwGetTime() - time;
			chunkTime *= 1000;
			Log_debug << "Chunk created: " << chunkTime << "ms\n";*/

			drawMesh(context.chunks[i * NUM_CHUNK + j].mesh, context.mainShader, context.cam, model);
		}
	}
}
