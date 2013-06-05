#include "sVert_PN.h"

const D3D11_INPUT_ELEMENT_DESC sVert_PN::LAYOUT[2] = 
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const unsigned int sVert_PN::STRIDE = sizeof(sVert_PN);
const unsigned int sVert_PN::NUM_ELEMENTS = 2;

D3D11_INPUT_ELEMENT_DESC* sVert_PN::GetElementDescPtr()
{
	return const_cast<D3D11_INPUT_ELEMENT_DESC*>( &(LAYOUT[0]) );
}
