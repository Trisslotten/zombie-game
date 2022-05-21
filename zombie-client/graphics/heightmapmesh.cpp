#include "heightmapmesh.hpp"

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <vector>

#include "engine/heightmap.hpp"

using namespace Graphics;

void HeightmapMesh::Initialize() {
	std::vector<glm::vec2> vertices;

	float size = 1024.0f;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			float x0 = (i + 0.5f) / size;
			float y0 = (j + 0.5f) / size;
			float x1 = (i + 1.5f) / size;
			float y1 = (j + 1.5f) / size;
			vertices.push_back(glm::vec2(x0, y0));
			vertices.push_back(glm::vec2(x1, y0));
			vertices.push_back(glm::vec2(x0, y1));

			vertices.push_back(glm::vec2(x1, y1));
			vertices.push_back(glm::vec2(x0, y1));
			vertices.push_back(glm::vec2(x1, y0));
		}
	}

	m_vertex_count = vertices.size();

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
		glEnableVertexAttribArray(0);
	}
	glBindVertexArray(0);
}

void HeightmapMesh::Draw() {
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
}

void HeightmapTexture::Initialize(const Heightmap& heightmap) {
	glGenTextures(1, &m_texture_id);
	glBindTexture(GL_TEXTURE_2D, m_texture_id);

	auto size = heightmap.GetSize();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, size, size, 0, GL_RED, GL_FLOAT, heightmap.GetHeights().data());
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void HeightmapTexture::Bind(u32 texture_slot) {
	glActiveTexture(GL_TEXTURE0 + texture_slot);
	glBindTexture(GL_TEXTURE_2D, m_texture_id);
}
