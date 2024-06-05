cmake_minimum_required(VERSION 3.27)
project(MarsEngine)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 23)

# Subdirectories
add_subdirectory(libraries/glfw)

# Include directories
include_directories(libraries)
include_directories(libraries/glfw/include)
include_directories(libraries/glad/include)
include_directories(libraries/glm)

set(SOURCE_FILES source/main.cpp libraries/glad/src/glad.c
        source/classes/shader_program.cpp
        source/classes/shader_program.h
        source/utility/utility_functions.cpp
        source/utility/utility_functions.h
        source/classes/camera.cpp
        source/classes/camera.h
        libraries/stb_image.cpp
        source/classes/shader_manager.cpp
        source/classes/shader_manager.h
        source/classes/light_manager.cpp
        source/classes/light_manager.h
        source/rendering/model.cpp
        source/rendering/model.h
        source/rendering/mesh.cpp
        source/rendering/mesh.h
        source/assets/import_functions.cpp
        source/assets/import_functions.h
        source/rendering/rendering_functions.cpp
        source/rendering/rendering_functions.h
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

#Symbolic link between source asset/shader files for easier development
add_custom_command(TARGET ${PROJECT_NAME}
    PRE_BUILD COMMAND
    ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)
add_custom_command(TARGET ${PROJECT_NAME}
        PRE_BUILD COMMAND
        ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

#Replace symlink custom commands with this line if symbolic links don't work on your machine
#file(COPY assets DESTINATION .)
#file(COPY shaders DESTINATION .)

# Library linking
target_link_libraries(${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARY})
link_directories(libraries)
