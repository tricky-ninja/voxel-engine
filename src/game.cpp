#include "game.h"
#include "Logger.h"
#include "random"
#include "gameData.h"
#include <algorithm>

#include "imgui.h"

struct GameContext
{
	World world;
	Camera cam;
	Shader mainShader;
	GLFWwindow* window;
	bool renderWrieframe = false;
	Texture mainAtlas;
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
	context.cam.pos = { 1.0f, 128.0f, 5.0f };
	context.cam.up = { 0,1,0 };
	context.cam.turn(1200, -400);
	context.cam.updateVectors();

	context.world.update(context.cam.pos);

	context.window = window;

	context.cam.isActive = false;

	context.mainAtlas.loadFromFile(ASSETS_PATH "textures/atlas.png");

	permAssert_msg(context.mainShader.loadFromFile(ASSETS_PATH "shaders/main.vert", ASSETS_PATH "shaders/main.frag"), "Failed to load main shaders");
	
}

void update(float deltaTime)
{
	glClearColor(185 / 255.f, 233 / 255.f, 250 / 255.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (context.renderWrieframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLFWwindow* window = context.window;

	ImGuiIO& io = ImGui::GetIO();

	if (context.cam.isActive) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) && !io.WantCaptureMouse == GLFW_PRESS)
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
	{
		context.cam.moveSpeed = SPEED * 1.5;
		context.cam.fov = ZOOM * 1.15;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
	{
		context.cam.fov = ZOOM;
		context.cam.moveSpeed = SPEED;
	}

	context.world.update(context.cam.pos);
	context.world.generateTerrain();

	context.mainShader.bind();
	context.mainAtlas.bind(1);
	context.mainShader.setInt("texture_atlas", context.mainAtlas.slot);
	context.world.render(context.mainShader, context.cam);

	ImGui::Begin("Info");
	ImGui::Text("Hiii");
	ImGui::End();

	ImGui::Begin("Infoj");
	ImGui::Text("Hiii");
	ImGui::End();

}
