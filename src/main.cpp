#include "Logger.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "openGLError.h"
#include "Renderer.h"
#include "game.h"

#if PRODUCTION_BUILD == 0
#include <crtdbg.h>
#endif

#define REMOVE_IMGUI 0

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imguiThemes.h"



const unsigned WIDTH =  1000;
const unsigned HEIGHT = 860;

void log_renderer_info() {
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
#if PRODUCTION_BUILD == 0
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Logger::set_log_level(LOG_DEBUG);
#if PRODUCTION_BUILD == 1
	Logger::set_log_level(LOG_INFO);
#endif
	Logger::set_output_file(LOGS_PATH "log.txt"); // will output to stdout if the logs folder is not created in development mode

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

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Voxel Engine", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		perm_assert_msg(false, "Failed to create window");
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1); // Enable vsync

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	perm_assert_msg(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize glad");
	
#pragma region imgui
	ImGui::CreateContext();
	imguiThemes::coolBlack();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows


	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.Colors[ImGuiCol_WindowBg].w = 0.f;
		style.Colors[ImGuiCol_DockingEmptyBg].w = 0.f;
	}

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

#pragma endregion

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

	Log_debug << "Max size of one chunk: " << (sizeof(PackedVertexData) * ((16*16*256) / 2 * 36))/(1024 * 1024) << "MB\n";
	float prevTime = 0.f, deltaTime = 0.f, currentTime = 0;
	init(window);

	while (!glfwWindowShouldClose(window))
	{

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwPollEvents();
		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		prevTime = currentTime;
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
		update(deltaTime);

#pragma region imgui
#if REMOVE_IMGUI == 0
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
#endif
#pragma endregion

		glfwSwapBuffers(window);
	}
	close();
	return 0;
}
