file(GLOB_RECURSE zombie_server_srcs *.cpp *.hpp)

add_executable(ZombieServer ${zombie_server_srcs})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${zombie_server_srcs})

target_include_directories(ZombieServer PRIVATE ./)
target_link_libraries(ZombieServer PRIVATE ZombieShared)
