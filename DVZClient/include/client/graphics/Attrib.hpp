#pragma once

#ifndef DVZ_ATTRIB_HPP
#define DVZ_ATTRIB_HPP

#include <glm/glm.hpp>

#include "core/common.hpp"
#include "glsl/preamble.glsl"

namespace DVZ::Graphics {
	namespace Detail {
		template<typename T>
		struct AttribType {
			typedef T ScalarType;
		};

		template<u8 SIZE>
		struct AttribSize {
			constexpr static u8 NumComponents = SIZE;
		};


		template<typename T> struct AttribInfo {};
		template<> struct AttribInfo<f64> : AttribType<f64>, AttribSize<1> {};
		template<> struct AttribInfo<f32> : AttribType<f32>, AttribSize<1> {};
		template<> struct AttribInfo<i64> : AttribType<i64>, AttribSize<1> {};
		template<> struct AttribInfo<u64> : AttribType<u64>, AttribSize<1> {};
		template<> struct AttribInfo<i32> : AttribType<i32>, AttribSize<1> {};
		template<> struct AttribInfo<u32> : AttribType<u32>, AttribSize<1> {};
		template<> struct AttribInfo<i16> : AttribType<i16>, AttribSize<1> {};
		template<> struct AttribInfo<u16> : AttribType<u16>, AttribSize<1> {};
		//template<> struct AttribInfo<i8> : AttribType<i8>, AttribSize<1> {};
		template<> struct AttribInfo<u8> : AttribType<u8>, AttribSize<1> {};

		template<typename T, glm::qualifier Q> struct AttribInfo<glm::vec<1, T, Q>> : AttribType<T>, AttribSize<1> {};
		template<typename T, glm::qualifier Q> struct AttribInfo<glm::vec<2, T, Q>> : AttribType<T>, AttribSize<2> {};
		template<typename T, glm::qualifier Q> struct AttribInfo<glm::vec<3, T, Q>> : AttribType<T>, AttribSize<3> {};
		template<typename T, glm::qualifier Q> struct AttribInfo<glm::vec<4, T, Q>> : AttribType<T>, AttribSize<4> {};
	}

	template<typename T>
	GLenum getGLType();


	template<u32 LOC, typename T, bool NORMALIZE = false>
	class Attrib {
	public:
		typename typedef T Type;
		typename typedef Detail::AttribInfo<T>::ScalarType ScalarType;
		constexpr static u8 NumComponents = Detail::AttribInfo<T>::NumComponents;
		constexpr static u32 Location = LOC;
		constexpr static bool Normalize = NORMALIZE;

		constexpr static size_t size() {
			return sizeof(T);
		}
	};

	typedef Attrib<POSITION_ATTRIB_LOCATION, glm::vec2> Position2DAttrib;
	typedef Attrib<POSITION_ATTRIB_LOCATION, glm::vec3> PositionAttrib;
	typedef Attrib<TEXCOORD_ATTRIB_LOCATION, glm::vec2> TexcoordAttrib;
	typedef Attrib<NORMAL_ATTRIB_LOCATION, glm::vec3> NormalAttrib;
	typedef Attrib<COLOR_ATTRIB_LOCATION, glm::vec3> ColorAttrib;

	//Particle Attribs
	typedef Attrib<TEX_INDEX_ATTRIB_LOCATION, glm::vec2> TexIndexAttrib;
	typedef Attrib<POSITION_SIZE_ATTRIB_LOCATION, glm::vec4> PositionSizeAttrib;
	typedef Attrib<COLOR_ATTRIB_LOCATION, glm::u8vec4, true> ParticleColorAttrib;
	typedef Attrib<LIFE_TIME_LOCATION, glm::f32> LifeTimeAttrib;
}

#endif