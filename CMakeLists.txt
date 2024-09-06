cmake_minimum_required(VERSION 3.16)

# DELETE THE OUT FOLDER AFTER CHANGING THIS
option(PRODUCTION_BUILD "Enable this to ship the exe" OFF)

if(MSVC) 
	# Static linking the runtime libraries
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	add_compile_options(/arch:AVX2) #make sure SIMD optimizations take place
endif()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) #link time optimizations

project(voxel-engine)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/stb_image)
add_subdirectory(thirdparty/imgui-docking)

file(GLOB_RECURSE SOURCE_FILES CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable("${CMAKE_PROJECT_NAME}")

set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY CXX_STANDARD 20)

if(PRODUCTION_BUILD)
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC ASSETS_PATH="./assets/")
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC LOGS_PATH="./")
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1) 
else()
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC LOGS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/logs/")
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=0) 
endif()

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${SOURCE_FILES})

if(MSVC)
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS) # disable warnings from being shown as errors
	if(PRODUCTION_BUILD)
		set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") #no console
	endif()
endif()

target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glm glfw glad stb_image imgui)
