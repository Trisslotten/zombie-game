#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

struct Transform {
	glm::quat orientation = glm::identity<glm::quat>();
	glm::vec3 position = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
};
