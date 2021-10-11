#include "client/graphics/GeometryBuilder.hpp"

using namespace DVZ::Graphics;

Geometry<PositionAttrib, TexcoordAttrib> DVZ::Graphics::gen_quad(float width) {
	Geometry<PositionAttrib, TexcoordAttrib> geometry;

	float h = width / 2;

	geometry.pushVertex(glm::vec3(-h, h, 0), glm::vec2(0, 0));
	geometry.pushVertex(glm::vec3(-h, -h, 0), glm::vec2(0, 1));
	geometry.pushVertex(glm::vec3(h, h, 0), glm::vec2(1, 0));
	geometry.pushVertex(glm::vec3(h, -h, 0), glm::vec2(1, 1));

	geometry.pushTriangle(0, 1, 2);
	geometry.pushTriangle(2, 1, 3);

	return geometry;
}

Geometry<Position2DAttrib, TexcoordAttrib> DVZ::Graphics::gen_quad2D(float width) {
	Geometry<Position2DAttrib, TexcoordAttrib> geometry;

	float h = width / 2;

	geometry.pushVertex(glm::vec2(-h, h), glm::vec2(0, 0));
	geometry.pushVertex(glm::vec2(-h, -h), glm::vec2(0, 1));
	geometry.pushVertex(glm::vec2(h, h), glm::vec2(1, 0));
	geometry.pushVertex(glm::vec2(h, -h), glm::vec2(1, 1));

	geometry.pushTriangle(0, 1, 2);
	geometry.pushTriangle(2, 1, 3);

	return geometry;
}

//this is really bad code. pls dont look at it
Geometry<PositionAttrib, TexcoordAttrib, NormalAttrib> DVZ::Graphics::gen_cube(float width) {
	Geometry<PositionAttrib, TexcoordAttrib, NormalAttrib> geometry;
	float h = width / 2;

	auto gen_face = [&geometry, h](int component, int first, int second, float sign) {

		//int first = (component + 1) % 3;
		//int second = (component + 2) % 3;

		{
			glm::vec3 pos;
			pos[first] = -h;
			pos[second] = h;
			pos[component] = sign * h;

			glm::vec2 tex;
			tex[0] = 0;
			tex[1] = 0;

			glm::vec3 norm(0.0f);
			norm[component] = sign;

			geometry.pushVertex(pos, tex, norm);
		}

		{
			glm::vec3 pos;
			pos[first] = -h;
			pos[second] = -h;
			pos[component] = sign * h;

			glm::vec2 tex;
			tex[0] = 0;
			tex[1] = 1;

			glm::vec3 norm(0.0f);
			norm[component] = sign;

			geometry.pushVertex(pos, tex, norm);
		}

		{
			glm::vec3 pos;
			pos[first] = h;
			pos[second] = h;
			pos[component] = sign * h;

			glm::vec2 tex;
			tex[0] = 1;
			tex[1] = 0;

			glm::vec3 norm(0.0f);
			norm[component] = sign;

			geometry.pushVertex(pos, tex, norm);
		}

		{
			glm::vec3 pos;
			pos[first] = h;
			pos[second] = -h;
			pos[component] = sign * h;

			glm::vec2 tex;
			tex[0] = 1;
			tex[1] = 1;

			glm::vec3 norm(0.0f);
			norm[component] = sign;

			geometry.pushVertex(pos, tex, norm);
		}

		auto offset = (u32)geometry.getVertexCount() - 4;
		geometry.pushTriangle(offset + 0, offset + 1, offset + 2);
		geometry.pushTriangle(offset + 2, offset + 1, offset + 3);

		//geometry.pushVertex(glm::vec4);
	};

	gen_face(0, 1, 2, 1.0f);
	gen_face(0, 2, 1, -1.0f);

	gen_face(1, 2, 0, 1.0f);
	gen_face(1, 0, 2, -1.0f);

	gen_face(2, 0, 1, 1.0f);
	gen_face(2, 1, 0, -1.0f);


	return geometry;
}