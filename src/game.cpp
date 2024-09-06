#include "game.h"
#include "Logger.h"
#include "random"
#include "gameData.h"

struct GameContext
{
	Chunk chunk;
	Camera cam;
	Shader mainShader;
	GLFWwindow* window;
	Mesh chunkMesh;
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
	context.cam;
	context.cam.pos = { 1.0f, 64.0f, 5.0f };
	context.cam.up = { 0,1,0 };
	context.cam.turn(1200, -400);
	context.cam.updateVectors();

	for (int x = 0; x < 64; x++)
	{
		for (int z = 0; z < 64; z++)
		{
			unsigned height = abs((sin(x / 16.0f) * 10.0f + sin(z / 16.0f) * 10.0f)    // Large rolling hills
				+ (sin(x / 8.0f) * 5.0f * cos(z / 8.0f) * 5.0f)       // Smaller details
				+ (sin(x / 4.0f) * 2.0f * cos(z / 4.0f) * 2.0f));      // Fine noise-like bumps
			height += 20.0f;
			for (int y = 0; y < 64; y++)
			{
				if (y < height) context.chunk.setBlock(true, x, y, z);
				else context.chunk.setBlock(false, x, y, z);
			}
		}
	}

	context.window = window;

	context.cam.isActive = false;
	context.chunkMesh = context.chunk.generateMesh();

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

	drawMesh(context.chunkMesh, context.mainShader, context.cam);
}
