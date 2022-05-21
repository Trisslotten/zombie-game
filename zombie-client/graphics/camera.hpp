#pragma once

#include <glm/ext/quaternion_float.hpp>

namespace Graphics {
struct Camera {
	glm::vec3 position{};
	glm::quat orientation = glm::identity<glm::quat>();
};
}
