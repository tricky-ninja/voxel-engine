#include "Logger.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "openGLError.h"
#include "Renderer.h"
#include "game.h"

const unsigned WIDTH =  1000;
const unsigned HEIGHT = 860;

int main()
{
	Logger::setLogLevel(LOG_DEBUG);
#if PRODUCTION_BUILD == 1
	Logger::setLogLevel(LOG_INFO);
#endif
	Logger::setOutputFile(LOGS_PATH "log.txt"); // will output to stdout if the logs folder is not created in development mode

#pragma region init

	if (!glfwInit())
	{
		std::cout << "Failed to initialise glfw\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Minecraft", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		permAssert_msg(false, "Failed to create window");
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1); // Enable vsync

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	permAssert_msg(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize glad");

	// Sets up error reporting
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}


	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	Log_debug << "Loaded openGL\n";

#pragma endregion

	init(window);
	float prevTime = 0.f, deltaTime = 0.f, currentTime = 0;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		prevTime = currentTime;
		update(deltaTime);

		glfwSwapBuffers(window);
	}

	return 0;
}
