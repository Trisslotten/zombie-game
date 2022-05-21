file(GLOB_RECURSE zombie_shared_srcs *.cpp *.hpp)

add_library(ZombieShared ${zombie_shared_srcs})

set_target_properties(ZombieShared PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${zombie_shared_srcs})

target_include_directories(ZombieShared PUBLIC ./)
target_link_libraries(ZombieShared PUBLIC glm::glm robin_hood sfml-network)
