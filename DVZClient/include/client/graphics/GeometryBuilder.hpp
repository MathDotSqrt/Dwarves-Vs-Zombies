#pragma once

#ifndef DVZ_GEOMETRYBUILDER_HPP
#define DVZ_GEOMETRYBUILDER_HPP

#include "client/graphics/Attrib.hpp"
#include "client/graphics/Geometry.hpp"

namespace DVZ::Graphics {
	Geometry<PositionAttrib, TexcoordAttrib> gen_quad(float width = 1);
	Geometry<Position2DAttrib, TexcoordAttrib> gen_quad2D(float width = 1);

	Geometry<PositionAttrib, TexcoordAttrib, NormalAttrib> gen_cube(float width);
}
#endif