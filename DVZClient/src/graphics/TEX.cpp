#include "client/graphics/TEX.hpp"

#include <cmath>
#include <utility>
#include <assert.h>
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace DVZ::Graphics;

TEX::TEX(Builder& builder) :
	texID(builder.texID),
	textureTarget(builder.textureTarget),
	width(builder.width),
	height(builder.height) {}

TEX::TEX(TEX&& other) {
	texID = other.texID;
	textureTarget = other.textureTarget;
	width = other.width;
	height = other.height;

	other.texID = 0;
}

TEX::~TEX() {
	dispose();
}

TEX& TEX::operator=(TEX&& other) {
	if (this != &other) {
		dispose();
		std::swap(textureTarget, other.textureTarget);
		std::swap(width, other.width);
		std::swap(height, other.height);
		std::swap(texID, other.texID);
	}

	return *this;
}

void TEX::bind() const {
	glBindTexture(textureTarget, texID);
}

void TEX::bindActiveTexture(int unit) const {
	if (unit >= 0) {
		glActiveTexture(GL_TEXTURE0 + unit);
		bind();
	}
}

void TEX::unbind() const {
	glBindTexture(textureTarget, 0);
}

void TEX::dispose() {
	if (texID) {
		glDeleteTextures(1, &texID);
		texID = 0;
	}
}

GLuint TEX::getTexID() const {
	return texID;
}

int TEX::getWidth() const {
	return width;
}

int TEX::getHeight() const {
	return height;
}

TEX::Builder::Builder() {
	mipmap = false;
	useMipMap = false;
	rgb().repeat().nearest().borderColor(1, 0, 1, 1).unsignedByteType();
}

TEX::Builder& TEX::Builder::a() {
	components = GL_ALPHA;
	storage = GL_ALPHA;

	return *this;
}

TEX::Builder& TEX::Builder::r() {
	components = GL_RED;
	//storage = GL_R16F;
	storage = GL_RED;
	return *this;
}

TEX::Builder& TEX::Builder::rgb() {
	components = GL_RGB;
	storage = GL_RGB;
	return *this;
}

TEX::Builder& TEX::Builder::rgba() {
	components = GL_RGBA;
	storage = GL_RGBA;
	return *this;
}

TEX::Builder& TEX::Builder::rgb16f() {
	components = GL_RGB;
	storage = GL_RGB16F;
	return *this;
}

TEX::Builder& TEX::Builder::depth24() {
	components = GL_DEPTH_COMPONENT;
	storage = GL_DEPTH_COMPONENT24;
	return *this;
}

TEX::Builder& TEX::Builder::unsignedByteType() {
	dataType = GL_UNSIGNED_BYTE;
	return *this;
}

TEX::Builder& TEX::Builder::floatType() {
	dataType = GL_FLOAT;

	return *this;
}



TEX::Builder& TEX::Builder::multisample(GLsizei samples) {
	this->samples = samples;

	return *this;
}

TEX::Builder& TEX::Builder::repeat() {
	wrapS = GL_REPEAT;
	wrapT = GL_REPEAT;
	return *this;
}

TEX::Builder& TEX::Builder::mirrorRepeat() {
	wrapS = GL_MIRRORED_REPEAT;
	wrapT = GL_MIRRORED_REPEAT;
	return *this;
}

TEX::Builder& TEX::Builder::clampToEdge() {
	wrapS = GL_CLAMP_TO_EDGE;
	wrapT = GL_CLAMP_TO_EDGE;
	return *this;
}

TEX::Builder& TEX::Builder::clampToBorder() {
	wrapS = GL_CLAMP_TO_BORDER;
	wrapT = GL_CLAMP_TO_BORDER;
	return *this;
}

TEX::Builder& TEX::Builder::borderColor(float r, float g, float b, float a) {
	bgColor[0] = r;
	bgColor[1] = g;
	bgColor[2] = b;
	bgColor[3] = a;
	return *this;
}

TEX::Builder& TEX::Builder::nearest() {
	filter = GL_NEAREST;
	return *this;
}

TEX::Builder& TEX::Builder::linear() {
	filter = GL_LINEAR;
	return *this;
}

TEX::Builder& TEX::Builder::mipmapNearest() {
	mipmap = GL_LINEAR;
	useMipMap = true;
	return *this;
}

TEX::Builder& TEX::Builder::mipmapLinear() {
	mipmap = GL_LINEAR;
	useMipMap = true;
	return *this;
}

TEX TEX::Builder::buildTexture(std::string filename) {
	u8* image = nullptr;
	if (filename != "") {
		int channels;
		image = stbi_load(filename.c_str(), &width, &height, &channels, components == GL_RGB ? 3 : 4);
		assert(image);
	}

	textureTarget = GL_TEXTURE_2D;

	int mipmapLevelCount = 0;

	glGenTextures(1, &texID);
	glBindTexture(textureTarget, texID);
	glTexImage2D(textureTarget, mipmapLevelCount, storage, width, height, 0, components, dataType, image);

	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameterfv(textureTarget, GL_TEXTURE_BORDER_COLOR, bgColor);

	GLenum f;

	if (useMipMap) {
		glGenerateMipmap(textureTarget);

		u8 bit1 = mipmap == GL_LINEAR;
		u8 bit2 = filter == GL_LINEAR;
		switch ((bit2 << 1) | bit1) {
		case 0x0: f = GL_NEAREST_MIPMAP_NEAREST; break;
		case 0x1: f = GL_LINEAR_MIPMAP_NEAREST; break;
		case 0x2: f = GL_NEAREST_MIPMAP_LINEAR; break;
		case 0x3: f = GL_LINEAR_MIPMAP_LINEAR; break;
		}
	}
	else {
		f = filter;
	}

	glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, f);
	glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, filter);

	stbi_image_free(image);

	glBindTexture(textureTarget, 0);

	return TEX(*this);
}

TEX TEX::Builder::buildTexture(int width, int height) {
	textureTarget = samples == 1 ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;

	int mipmapLevelCount = 0;

	glGenTextures(1, &texID);
	glBindTexture(textureTarget, texID);

	if (samples == 1) {
		glTexImage2D(textureTarget, mipmapLevelCount, storage, width, height, 0, components, dataType, nullptr);
	}
	else {
		glTexImage2DMultisample(textureTarget, samples, storage, width, height, GL_TRUE);
	}

	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameterfv(textureTarget, GL_TEXTURE_BORDER_COLOR, bgColor);

	GLenum f;

	if (useMipMap) {
		glGenerateMipmap(textureTarget);

		u8 bit1 = mipmap == GL_LINEAR;
		u8 bit2 = filter == GL_LINEAR;
		switch ((bit2 << 1) | bit1) {
		case 0x0: f = GL_NEAREST_MIPMAP_NEAREST; break;
		case 0x1: f = GL_LINEAR_MIPMAP_NEAREST; break;
		case 0x2: f = GL_NEAREST_MIPMAP_LINEAR; break;
		case 0x3: f = GL_LINEAR_MIPMAP_LINEAR; break;
		}
	}
	else {
		f = filter;
	}

	glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, f);
	glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, filter);

	glBindTexture(textureTarget, 0);

	return TEX(*this);
}

TEX TEX::Builder::buildTexture3D(const std::vector<float>& buffer) {
	const f32* image = buffer.data();
	int width = (int)std::cbrt(buffer.size());

	textureTarget = GL_TEXTURE_3D;
	int mipmapLevelCount = 0;

	glGenTextures(1, &texID);
	glBindTexture(textureTarget, texID);
	glTexImage3D(textureTarget, mipmapLevelCount, storage, width, width, width, 0, components, dataType, image);

	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, wrapT);
	//glTexParameterfv(textureTarget, GL_TEXTURE_BORDER_COLOR, bgColor);

	GLenum f;

	if (useMipMap) {
		glGenerateMipmap(textureTarget);

		u8 bit1 = mipmap == GL_LINEAR;
		u8 bit2 = filter == GL_LINEAR;
		switch ((bit2 << 1) | bit1) {
		case 0x0: f = GL_NEAREST_MIPMAP_NEAREST; break;
		case 0x1: f = GL_LINEAR_MIPMAP_NEAREST; break;
		case 0x2: f = GL_NEAREST_MIPMAP_LINEAR; break;
		case 0x3: f = GL_LINEAR_MIPMAP_LINEAR; break;
		}
	}
	else {
		f = filter;
	}

	glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, f);
	glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, filter);
	glBindTexture(textureTarget, 0);

	return TEX(*this);
}