#include "core/voxel/block.hpp"

using namespace DVZ::Voxel;

BlockTexIndices::BlockTexIndices(TexIndex all) : 
	px(all), 
	nx(all),
	py(all),
	ny(all),
	pz(all),
	nz(all)
{}

BlockTexIndices::BlockTexIndices(TexIndex top, TexIndex side, TexIndex bottom) :
	px(side),
	nx(side),
	py(top),
	ny(bottom),
	pz(side),
	nz(side)
{}

BlockTexIndices::BlockTexIndices(TexIndex px, TexIndex nx, TexIndex py, TexIndex ny, TexIndex pz, TexIndex nz) :
	px(px),
	nx(nx),
	py(py),
	ny(ny),
	pz(pz),
	nz(nz)
{}