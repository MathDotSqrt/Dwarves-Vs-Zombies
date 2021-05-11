#pragma once 

#ifndef DVZ_TEX_HPP
#define DVZ_TEX_HPP

#include <string>
#include <vector>
#include "core/common.hpp"

namespace DVZ::Graphics {
	class TEX {
	public:
		TEX(TEX&&);
		~TEX();
		TEX(const TEX&) = delete;

		TEX& operator=(const TEX&) = delete;
		TEX& operator=(TEX&& other);

		void bind() const;
		void bindActiveTexture(int unit) const;
		void unbind() const;

		GLuint getTexID() const;
		int getWidth() const;
		int getHeight() const;

		class Builder {
		private:
			friend class TEX;
			int width, height;

			GLuint texID;
			GLenum textureTarget;

			GLsizei samples = 1;
			GLenum storage;
			GLenum components;
			GLenum dataType;

			GLenum wrapS;
			GLenum wrapT;

			float bgColor[4];

			GLenum filter;
			GLenum mipmap;
			bool useMipMap;

		public:
			Builder();

			Builder& a();
			Builder& r();
			Builder& rgb();
			Builder& rgba();
			Builder& rgb16f();
			Builder& depth24();
			Builder& unsignedByteType();
			Builder& floatType();

			Builder& multisample(GLsizei samples = 4);
			Builder& repeat();
			Builder& mirrorRepeat();
			Builder& clampToEdge();
			Builder& clampToBorder();
			Builder& borderColor(float r, float g, float b, float a);
			Builder& nearest();
			Builder& linear();
			Builder& mipmapNearest();
			Builder& mipmapLinear();

			TEX buildTexture(std::string filename);
			TEX buildTexture(int width, int height);
			TEX buildTexture3D(const std::vector<float>& buffer);
		};

	private:
		GLuint texID;
		GLenum textureTarget;
		int width, height;

		TEX(Builder& builder);

		void dispose();
	};
}

#endif