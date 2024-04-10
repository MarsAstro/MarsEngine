cmake_minimum_required(VERSION 3.27)
project(MarsEngine)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 23)

# Subdirectories
add_subdirectory(libraries/glfw)
add_subdirectory(libraries/assimp)

# Include directories
include_directories(libraries)
include_directories(libraries/glfw/include)
include_directories(libraries/assimp/include)
include_directories(libraries/glad/include)
include_directories(libraries/glm)

set(ASSIMP_BUILD_STATIC_LIB ON)
set(SOURCE_FILES source/main.cpp libraries/glad/src/glad.c
        source/classes/shader.cpp
        source/classes/shader.h
        source/classes/mesh.cpp
        source/classes/mesh.h
        source/classes/model.cpp
        source/classes/model.h
        source/functions.cpp
        source/functions.h
        source/classes/camera.cpp
        source/classes/camera.h
        source/classes/point_light.cpp
        source/classes/point_light.h
        source/classes/point_light_collection.cpp
        source/classes/point_light_collection.h
        libraries/stb_image.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
file(COPY assets DESTINATION .)
file(COPY shaders DESTINATION .)


# Library linking
target_link_libraries(${CMAKE_PROJECT_NAME} glfw assimp ${GLFW_LIBRARIES} ${OPENGL_LIBRARY})
link_directories(libraries)
