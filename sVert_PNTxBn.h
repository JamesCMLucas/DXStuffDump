#ifndef __JL_sVert_PNTxBn_HG_20130317__
#define __JL_sVert_PNTxBn_HG_20130317__

#include <D3D11.h>
#include <cVector4.h>

/// sBone Space: Bone Indices | Bone Weights
struct sBoneSpace
{
	sBoneSpace()
	{
		BoneIndices[0] = 0;
		BoneIndices[1] = 0;
		BoneIndices[2] = 0;
		BoneIndices[3] = 0;
		BoneWeights = cVector4(0.f,0.f,0.f,0.f);
	}
	unsigned int BoneIndices[4];
	cVector4 BoneWeights;
};

/// Position | Normal | Tex0 | Tex1 | sBoneSpace
struct sVert_PNTxBn
{
	sVert_PNTxBn() : PositionPad(1.f), NormalPad(1.f) {}
	cVector3 Position;
	float PositionPad;
	cVector3 Normal;
	float NormalPad;
	cVector2 TexCoord0;
	cVector2 TexCoord1;
	sBoneSpace BoneSpace;
	

	static D3D11_INPUT_ELEMENT_DESC* GetElementDescPtr();

	static const unsigned int NUM_ELEMENTS;
	static const D3D11_INPUT_ELEMENT_DESC LAYOUT[6];
	static const unsigned int STRIDE;
};

#endif  //  __JL_sVert_PNTxBn_HG_20130317__