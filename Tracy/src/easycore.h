#pragma once

class EasyCore : public RenderCore {
public:
	EasyCore(uint32_t windowWidth, uint32_t windowHeight);

	void setVertexData(const std::vector<float>& vertices) override;
	void setTriangleData(const std::vector<uint32_t>& triangles) override;
	void setQuadData(const std::vector<uint32_t>& quads) override;
	void setSphereData(const std::vector<std::tuple<uint32_t, float>>& spheres) override;
	const Frame& nextFrame() override;

private:
	std::vector<float> m_VertexData;
	std::vector<uint32_t> m_TriangleData;
	std::vector<std::tuple<uint32_t, float>> m_SphereData;

	Frame m_Frame;
};