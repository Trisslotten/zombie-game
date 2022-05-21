#include "heightmap.hpp"

#include <iostream>

void Heightmap::Generate() {
	//fnl_state state = fnlCreateState();
	//state.frequency = 0.01f;
	//state.fractal_type = FNL_FRACTAL_FBM;

	m_size = 256;
	m_heights.resize(m_size * m_size);

	for (i64 y = 0; y < m_size; ++y) {
		for (i64 x = 0; x < m_size; ++x) {
			i64 index = x + y * m_size;
			//m_heights[index] = fnlGetNoise2D(&state, x, y);
			m_heights[index] = 0.f;
		}
	}
}

u32 Heightmap::GetSize() const {
	return m_size;
}

const std::vector<float>& Heightmap::GetHeights() const {
	return m_heights;
}
