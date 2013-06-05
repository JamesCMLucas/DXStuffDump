#ifndef __JL_sVert_PN_HG_20130317__
#define __JL_sVert_PN_HG_20130317__

#include <D3D11.h>
#include <cVector3.h>

/// Position | Normal
struct sVert_PN
{
	sVert_PN() : PositionPad(1.f), NormalPad(1.f) {}
	cVector3 Position;
	float PositionPad;
	cVector3 Normal;
	float NormalPad;

	static D3D11_INPUT_ELEMENT_DESC* GetElementDescPtr();

	static const unsigned int NUM_ELEMENTS;
	static const D3D11_INPUT_ELEMENT_DESC LAYOUT[2];
	static const unsigned int STRIDE;
};

#endif  //  __JL_sVert_PN_HG_20130317__
