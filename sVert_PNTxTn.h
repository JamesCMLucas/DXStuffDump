#ifndef __JL_sVert_PNTxTn_HG_20130317__
#define __JL_sVert_PNTxTn_HG_20130317__

#include <D3D11.h>
#include <cVector4.h>

/// Position | Normal | Tex0 | Tex1 | Tangent
struct sVert_PNTxTn
{
	sVert_PNTxTn() : PositionPad(1.f), NormalPad(1.f), TangentPad(1.f) {}

	cVector3 Position;
	float PositionPad;
	cVector3 Normal;
	float NormalPad;
	cVector2 TexCoord0;
	cVector2 TexCoord1;
	cVector3 Tangent;
	float TangentPad;


	static D3D11_INPUT_ELEMENT_DESC* GetElementDescPtr();

	static const unsigned int NUM_ELEMENTS;
	static const D3D11_INPUT_ELEMENT_DESC LAYOUT[5];
	static const unsigned int STRIDE;
};

#endif  //  __JL_sVert_PNTxTnBt_HG_20130317__