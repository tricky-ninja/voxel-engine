#include "game.h"
#include "Logger.h"
#include "random"
#include "gameData.h"
#include <algorithm>
#include <Quad.h>

#include "imgui.h"

#include <glm/glm.hpp>

void get_chunk_and_block_coords(const glm::vec3& cameraPosition, int CHUNK_SIZE,
	glm::ivec2& chunkCoord, glm::ivec3& blockCoord)
{
	chunkCoord.x = (int)(floor(cameraPosition.x / CHUNK_SIZE));
	chunkCoord.y = (int)(floor(cameraPosition.z / CHUNK_SIZE));

	int blockX = (int)(floor(cameraPosition.x)) % CHUNK_SIZE;
	int blockZ = (int)(floor(cameraPosition.z)) % CHUNK_SIZE;
	int blockY = (int)(cameraPosition.y);

	// Correct for negative modulo to ensure block coordinates are within 0 - chunksize
	if (blockX < 0) blockX += CHUNK_SIZE;
	if (blockZ < 0) blockZ += CHUNK_SIZE;

	blockCoord = glm::ivec3(blockX, blockY, blockZ);
}


struct MouseContext
{
	float lastX = 0;
	float lastY = 0;
};

struct GameContext
{
	World world;
	Camera cam;
	Shader terrainShader;
	Shader screenShader;
	Shader waterShader;
	GLFWwindow* window;
	bool renderWrieframe = false;
	Texture mainAtlas;
	MouseContext mouse;
	Framebuffer gameBuffer;
	unsigned width, height;
	bool underWater = false;
	bool enableFog = true;
};

GameContext *context;



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_F3:
			context->renderWrieframe = !context->renderWrieframe;
			break;
		
		}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	context->width = width;
	context->height = height;
	context->gameBuffer = Framebuffer(width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float xoffset = xpos - context->mouse.lastX;
	float yoffset = context->mouse.lastY - ypos;
	context->mouse.lastX = xpos;
	context->mouse.lastY = ypos;
	if (context->cam.isActive) context->cam.turn(xoffset, yoffset);
}


void init(GLFWwindow* window)
{
	srand(20);

	init_quad();

	int width=0, height=0;
	glfwGetWindowSize(window, &width, &height);
	
	
	context = new GameContext();

	context->gameBuffer = Framebuffer(width, height);
	
	context->width = width;
	context->height = height;

	context->cam.pos = { 1.0f, 128.0f, 5.0f };
	context->cam.up = { 0,1,0 };
	context->cam.turn(1200, 0);
	context->cam.update_vectors();

	context->world.update_state(context->cam.pos);

	context->window = window;

	context->cam.isActive = false;

	context->mainAtlas.load_from_file(ASSETS_PATH "textures/atlas.png");

	perm_assert_msg(context->terrainShader.load_from_file(ASSETS_PATH "shaders/basic_terrain.vert", ASSETS_PATH "shaders/basic_terrain.frag"), "Failed to load main shaders");
	perm_assert_msg(context->screenShader.load_from_file(ASSETS_PATH "shaders/framebuffer.vert", ASSETS_PATH "shaders/framebuffer.frag"), "Failed to load main shaders");
	perm_assert_msg(context->waterShader.load_from_file(ASSETS_PATH "shaders/water.vert", ASSETS_PATH "shaders/water.frag"), "Failed to load main shaders");
	
}

void update(float deltaTime)
{


	context->gameBuffer.bind();
	glEnable(GL_DEPTH_TEST);
	glClearColor(185 / 255.f, 233 / 255.f, 250 / 255.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLineWidth(3);
	if (context->renderWrieframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLFWwindow* window = context->window;

	ImGuiIO& io = ImGui::GetIO();

	if (context->cam.isActive) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) && !io.WantCaptureMouse == GLFW_PRESS)
		context->cam.isActive = true;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		context->cam.isActive = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		context->cam.move(FORWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		context->cam.move(BACKWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		context->cam.move(LEFT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		context->cam.move(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		context->cam.move(UP, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		context->cam.move(DOWN, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		context->cam.moveSpeed = SPEED * 1.5;
		context->cam.fov = ZOOM * 1.15;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
	{
		context->cam.fov = ZOOM;
		context->cam.moveSpeed = SPEED;
	}

	glm::ivec2 chunkCoord;
	glm::ivec3 blockCoord;

	get_chunk_and_block_coords(context->cam.pos, CHUNK_SIZE, chunkCoord, blockCoord);

	if ((context->world.get_chunk(chunkCoord.x, chunkCoord.y) != nullptr) && context->world.get_chunk(chunkCoord.x, chunkCoord.y)->get_block_at(blockCoord.x, blockCoord.y, blockCoord.z) == WATER_BLOCK) context->underWater = true;
	else context->underWater = false;

	context->world.update_state(context->cam.pos);
	context->world.apply_updates();

	context->screenShader.hot_reload();
	context->terrainShader.hot_reload();
	context->waterShader.hot_reload();
	context->mainAtlas.hot_reload();

	context->terrainShader.bind();
	context->mainAtlas.bind(1);
	context->terrainShader.set_int("texture_atlas", context->mainAtlas.slot);
	context->terrainShader.set_bool("enableFog", context->enableFog);
	context->waterShader.bind();
	context->waterShader.set_int("texture_atlas", context->mainAtlas.slot);
	context->world.render(context->terrainShader, context->waterShader, context->cam);
	context->terrainShader.unbind();

	context->gameBuffer.unbind();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	context->screenShader.bind();
	context->screenShader.set_int("screenTexture", 0);
	context->screenShader.set_bool("inWater", context->underWater);
	context->screenShader.set_unsigned("width", context->width);
	context->screenShader.set_unsigned("height", context->height);
	context->screenShader.set_float("time", glfwGetTime());
	glBindVertexArray(quadVAO);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, context->gameBuffer.textureColorBuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	ImGui::Begin("Info");
	ImGui::Text("Click the window to look around, press esc to unfocus");
	ImGui::Text("WASD - Move");
	ImGui::Text("Ctrl(hold) - Increase speed");
	ImGui::Text("Shift/Space - Go down/up");
	ImGui::Text("F3 - Toggle wireframe");
	ImGui::Text("Q - Quit");
	ImGui::Text("%s", context->cam.get_coords_as_string().c_str());
	ImGui::Text("Chunk: %d, %d", chunkCoord.x, chunkCoord.y);
	ImGui::Text("Local Block: %d, %d, %d", blockCoord.x, blockCoord.y, blockCoord.z);
	ImGui::End();

	ImGui::Begin("Settings");
	ImGui::Checkbox("Wireframe", &context->renderWrieframe);
	ImGui::Checkbox("Fog", &context->enableFog);
	ImGui::Text("Render Distance");
	ImGui::SliderInt("##RenderDistance", &context->world.RENDER_DISTANCE, 2, 20);
	ImGui::Text("Chunks Rendered Per Frame");
	ImGui::SliderInt("##ChunksPerFrame", &context->world.maxChunksPerFrame, 1, 50);
	ImGui::End();

}

void close()
{
	context->world.delete_all();
	delete context;
}
