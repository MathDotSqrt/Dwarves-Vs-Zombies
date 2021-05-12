#pragma once 

#ifndef DVZ_MESH_HPP
#define DVZ_MESH_HPP

#include "client/graphics/VAO.hpp"
#include "client/graphics/VBO.hpp"
#include "client/graphics/Geometry.hpp"

namespace DVZ::Graphics {
	class Mesh {
	public:
		template<typename ...T>
		Mesh(const Geometry<T...>& geometry) :
			vbo(VBO::BufferType::ARRAY_BUFFER),
			ebo(VBO::BufferType::ELEMENT_ARRAY_BUFFER) {

			vao.bind();

			vbo.bind();
			vao.addVertexAttribPtr<T...>();
			vbo.bufferData(geometry.getVerticies());
			vbo.unbind();

			ebo.bind();
			ebo.bufferData(geometry.getIndices());
			ebo.unbind();

			vao.unbind();

		}

		VAO vao;
		VBO vbo;
		VBO ebo;
	};
}

#endif // !DVZ_MESH_HPP
