#ifndef __JL_sVert_P_HG_20130317__
#define __JL_sVert_P_HG_20130317__

#include <D3D11.h>
#include <cVector3.h>

// Position
struct sVert_P
{
	sVert_P() : PositionPad(1.f) {}
	cVector3 Position;
	float PositionPad;

	static D3D11_INPUT_ELEMENT_DESC* GetElementDescPtr();
	static const unsigned int NUM_ELEMENTS;
	static const D3D11_INPUT_ELEMENT_DESC LAYOUT[1];
	static const unsigned int STRIDE;
};

#endif  //  __JL_sVert_P_HG_20130317__
