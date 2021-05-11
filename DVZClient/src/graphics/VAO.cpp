#include "client/graphics/VAO.hpp"

#include <GL/glew.h>
#include <algorithm>
#include <iostream>
#include <spdlog/spdlog.h>

using namespace DVZ::Graphics;

VAO::VAO() {
	glGenVertexArrays(1, &vaoID);
	spdlog::debug("VAO: generated with ID [{}]", vaoID);
}

VAO::~VAO() {
	dispose();
}

VAO::VAO(VAO&& other) : vaoID(other.vaoID) {
	other.vaoID = 0;
}

VAO& VAO::operator=(VAO&& other) {
	if (this != &other) {
		dispose();

		std::swap(vaoID, other.vaoID);
	}

	return *this;
}

void VAO::bind() const {
	glBindVertexArray(vaoID);
}

void VAO::unbind() const {
	glBindVertexArray(0);
}

void VAO::addVertexAttribPtr(u32 ptr, u8 num_components, GLenum type, bool normalize, size_t stride, size_t offset) {
	glEnableVertexAttribArray(ptr);
	glVertexAttribPointer(ptr, num_components, type, normalize, stride, (void*)offset);
}

void VAO::vertexAttribDivisor(u32 ptr, u32 divisor) {
	glVertexAttribDivisor(ptr, divisor);
}

void VAO::dispose() {
	if (vaoID) {
		glDeleteVertexArrays(1, &vaoID);
		spdlog::debug("VAO: disposed with ID [{}]", vaoID);

		vaoID = 0;
	}
}

GLuint VAO::getID() const {
	return vaoID;
}