#pragma once

#include <vector>

#include "util/IntTypes.hpp"

class Heightmap {
public:

	void Generate();

	const std::vector<float>& GetHeights() const;
	u32 GetSize() const;

private:
	std::vector<float> m_heights;
	u32 m_size;
};
