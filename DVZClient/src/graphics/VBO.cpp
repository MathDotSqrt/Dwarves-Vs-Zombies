#include "client/graphics/VBO.hpp"

#include <assert.h>
#include <GL/glew.h>
#include <spdlog/spdlog.h>

using namespace DVZ::Graphics;

constexpr GLenum toGL(VBO::BufferType type) {
	switch (type)
	{
	case VBO::BufferType::ARRAY_BUFFER:
		return GL_ARRAY_BUFFER;
	case VBO::BufferType::ELEMENT_ARRAY_BUFFER:
		return GL_ELEMENT_ARRAY_BUFFER;
	default:
		assert(false);
		return 0;
	}
}

constexpr GLenum toGL(VBO::BufferHint type) {
	switch (type)
	{
	case VBO::BufferHint::STATIC_DRAW:
		return GL_STATIC_DRAW;
	case VBO::BufferHint::STREAM_DRAW:
		return GL_STREAM_DRAW;
	default:
		assert(false);
		return 0;
	}
}

VBO::VBO(VBO::BufferType type) : vboID(0), type(type), bytes(0) {
	glGenBuffers(1, &vboID);
	spdlog::debug("VBO[{}]: generated", vboID);
	assert(vboID);
}

VBO::VBO(VBO&& other) noexcept : vboID(other.vboID), type(other.type), bytes(other.bytes) {
	other.vboID = 0;
}

VBO::~VBO() {
	dispose();
}

VBO& VBO::operator=(VBO&& other) noexcept {
	if (this != &other) {
		dispose();
		std::swap(vboID, other.vboID);
		std::swap(type, other.type);
	}

	return *this;
}


void VBO::bind() const {
	assert(vboID);
	glBindBuffer(toGL(type), vboID);
}

void VBO::unbind() const {
	glBindBuffer(toGL(type), 0);
}

void VBO::bufferOrphan() {
	glBufferData(toGL(type), bytes, nullptr, GL_STREAM_DRAW);
}

void VBO::bufferData(size_t bytes, void* data, BufferHint hint) {
	assert(bytes != 0);
	spdlog::debug("VBO[{}]: buffered {} bytes", vboID, bytes);
	glBufferData(toGL(type), bytes, data, toGL(hint));
	this->bytes = bytes;
}

void VBO::bufferSubData(size_t offset, size_t bytes, void* data) {
	spdlog::debug("VBO[{}]: sub-buffered {} bytes", vboID, bytes);
	assert(bytes <= this->bytes);
	glBufferSubData(toGL(type), offset, bytes, data);
}

void VBO::dispose() {
	if (this->vboID) {
		glDeleteBuffers(1, &vboID);
		spdlog::debug("VBO[{}]: disposed", vboID);
		this->vboID = 0;
	}
}

GLuint VBO::getID() const {
	return vboID;
}

VBO::BufferType VBO::getType() const {
	return type;
}

size_t VBO::getNumBytes() const {
	return bytes;
}
