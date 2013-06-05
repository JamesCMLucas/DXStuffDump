#include "sVert_PNTxBn.h"

const D3D11_INPUT_ELEMENT_DESC sVert_PNTxBn::LAYOUT[6] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const unsigned int sVert_PNTxBn::STRIDE = sizeof(sVert_PNTxBn);
const unsigned int sVert_PNTxBn::NUM_ELEMENTS = 6;

D3D11_INPUT_ELEMENT_DESC* sVert_PNTxBn::GetElementDescPtr()
{
	return const_cast<D3D11_INPUT_ELEMENT_DESC*>( &(LAYOUT[0]) );
}