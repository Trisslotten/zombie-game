file(GLOB_RECURSE zombie_client_srcs *.cpp *.hpp)

add_executable(ZombieClient ${zombie_client_srcs})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${zombie_client_srcs})

target_include_directories(ZombieClient PRIVATE ./)
target_link_libraries(ZombieClient PRIVATE ZombieShared glfw glad)
add_dependencies(ZombieClient ZombieShaders)
