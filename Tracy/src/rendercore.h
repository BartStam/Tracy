#pragma once

struct Frame {
	std::vector<uint8_t> pixelData;
	uint32_t width;
	uint32_t height;
};

class RenderCore {
public:
	virtual void setVertexData(const std::vector<float>& vertices) = 0;
	virtual void setTriangleData(const std::vector<uint32_t>& triangles) = 0;
	virtual void setQuadData(const std::vector<uint32_t>& quads) = 0;
	virtual void setSphereData(const std::vector<std::tuple<uint32_t, float>>& spheres) = 0;
	virtual const Frame& nextFrame() = 0;
};