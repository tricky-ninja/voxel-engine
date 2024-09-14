#include "Logger.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "openGLError.h"
#include "Renderer.h"
#include "game.h"
#include <crtdbg.h>

const unsigned WIDTH =  1000;
const unsigned HEIGHT = 860;

void display() {
	// Get and print the GPU and OpenGL version info
	const GLubyte* renderer = glGetString(GL_RENDERER);  // Get the renderer (GPU) name
	const GLubyte* vendor = glGetString(GL_VENDOR);      // Get the vendor name
	const GLubyte* version = glGetString(GL_VERSION);    // Get the OpenGL version
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION); // GLSL version

	printf("GPU (Renderer): %s\n", renderer);
	printf("Vendor: %s\n", vendor);
	printf("OpenGL Version: %s\n", version);
	printf("GLSL Version: %s\n", glslVersion);

	// Print the number of supported extensions (OpenGL 3.0 and higher)
	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	printf("Number of Extensions: %d\n", numExtensions);

	for (int i = 0; i < numExtensions; i++) {
		printf("%s\n", glGetStringi(GL_EXTENSIONS, i));  // List all supported extensions
	}
}

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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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

	const GLubyte* renderer = glGetString(GL_RENDERER);  
	const GLubyte* vendor = glGetString(GL_VENDOR);      
	const GLubyte* version = glGetString(GL_VERSION);    
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	Log_info << "GPU (Renderer): " << renderer << "\n";
	Log_info << "Vendor: " << vendor << "\n";
	Log_info << "OpenGL Version: " << version << "\n";
	Log_info << "GLSL Version: " << glslVersion << "\n";

	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	Log_info << "Number of Extensions: " << numExtensions << "\n";

#pragma endregion

	Log_debug << "Size of one chunk: " << (sizeof(PackedVertexData) * ((16*16*128) / 2 * 36))/(1024 * 1024) << "MB\n";
	float prevTime = 0.f, deltaTime = 0.f, currentTime = 0;
	init(window);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		prevTime = currentTime;
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
		update(deltaTime);

		glfwSwapBuffers(window);
	}

	return 0;
}
