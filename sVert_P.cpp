#include "sVert_P.h"

const D3D11_INPUT_ELEMENT_DESC sVert_P::LAYOUT[1] = 
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const unsigned int sVert_P::STRIDE = sizeof(sVert_P);
const unsigned int sVert_P::NUM_ELEMENTS = 1;

D3D11_INPUT_ELEMENT_DESC* sVert_P::GetElementDescPtr()
{
	return const_cast<D3D11_INPUT_ELEMENT_DESC*>( &(LAYOUT[0]) );
}