cmake_minimum_required(VERSION 3.20)
include(FetchContent)


project(Zombie)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20)
# SFML uses auto_ptr
add_compile_definitions(_HAS_AUTO_PTR_ETC=1)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(
    ZombieShaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/shaders
    ${PROJECT_BINARY_DIR}/shaders
)

FetchContent_Declare(
    GLM_EXTERNAL
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e
)
FetchContent_Declare(
    ROBIN_HOOD_EXTERNAL
    GIT_REPOSITORY https://github.com/martinus/robin-hood-hashing.git
    GIT_TAG 9145f963d80d6a02f0f96a47758050a89184a3ed
)
set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_DOC OFF)
set(SFML_BUILD_AUDIO OFF)
set(SFML_BUILD_GRAPHICS OFF)
set(SFML_BUILD_WINDOW OFF)
set(SFML_BUILD_NETWORK ON)
FetchContent_Declare(
    SFML_EXTERNAL
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2f11710abc5aa478503a7ff3f9e654bd2078ebab
)
set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
FetchContent_Declare(
    GLFW_EXTERNAL
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 7482de6071d21db77a7236155da44c172a7f6c9e
)
FetchContent_Declare(
    GLAD_EXTERNAL
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG 1ecd45775d96f35170458e6b148eb0708967e402
)

FetchContent_MakeAvailable(
    GLM_EXTERNAL
    ROBIN_HOOD_EXTERNAL
    SFML_EXTERNAL
    GLFW_EXTERNAL
    GLAD_EXTERNAL
)

add_subdirectory(zombie-shared)
add_subdirectory(zombie-server)
add_subdirectory(zombie-client)
