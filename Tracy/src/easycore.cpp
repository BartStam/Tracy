#include "pch.h"

EasyCore::EasyCore(uint32_t windowWidth, uint32_t windowHeight) {
	uint32_t size = 3 * windowWidth * windowHeight;
	m_Frame.pixelData.resize(size, 128);
	m_Frame.width = windowWidth;
	m_Frame.height = windowHeight;
}

void EasyCore::setVertexData(const std::vector<float>& vertices) {

}

void EasyCore::setTriangleData(const std::vector<uint32_t>& triangles) {

}
void EasyCore::setQuadData(const std::vector<uint32_t>& quads) {

}

void EasyCore::setSphereData(const std::vector<std::tuple<uint32_t, float>>& spheres) {

}

const Frame& EasyCore::nextFrame() {
	return m_Frame;
}