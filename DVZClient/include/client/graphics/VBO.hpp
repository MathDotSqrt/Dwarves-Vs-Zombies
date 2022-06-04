#pragma once
#ifndef DVZ_VBO_HPP
#define DVZ_VBO_HPP

#include <vector>

#include "core/common.hpp"


namespace DVZ::Graphics {
	class VBO {
	public:

		enum class BufferType : GLenum {
			ARRAY_BUFFER,
			ELEMENT_ARRAY_BUFFER
		};

		enum class BufferHint : GLenum {
			STATIC_DRAW,
			STREAM_DRAW,
			DYNAMIC_DRAW
		};

		VBO(BufferType type);
		VBO(VBO&& other) noexcept;
		~VBO();

		VBO(const VBO&) = delete;
		VBO& operator =(const VBO&) = delete;
		VBO& operator=(VBO&& other) noexcept;

		void bind() const;
		void unbind() const;

		//do not call this before buffer data
		void bufferOrphan();
		void bufferData(size_t bytes, void* data, BufferHint hint = BufferHint::STATIC_DRAW);
		void bufferSubData(size_t offset, size_t bytes, void* data);

		template<typename T>
		void bufferData(const std::vector<T>& data, BufferHint hint = BufferHint::STATIC_DRAW) {
			void* data_ptr = (void*)data.data();
			size_t num_bytes = sizeof(T) * data.size();

			if (num_bytes > getNumBytes()) {
				bufferData(num_bytes, data_ptr, hint);
			}
			else {
				bufferSubData(0, num_bytes, data_ptr);
			}
		}

		template<typename T>
		void bufferSubData(const std::vector<T>& data) {
			void* data_ptr = (void*)data.data();
			bufferSubData(0, sizeof(T) * data.size(), data_ptr);
		}

		GLuint getID() const;
		BufferType getType() const;
		size_t getNumBytes() const;

	private:
		GLuint vboID;
		BufferType type;
		size_t bytes;	//in bytes

		void dispose();
	};
}

#endif