#ifndef __cHeightMapQueryEffect_HG_20130411__
#define __cHeightMapQueryEffect_HG_20130411__

#include "cDynamicBufferT.h"
#include "iEffect.h"
#include "DXBase.h"
#include "nCompute.h"


class cHeightMapQueryEffect : public iEffect
{
	
public:
	typedef iEffect Base;
	typedef nCompute::cOutputBufferT<float> cOutputBuffer;
	typedef nCompute::cInputBufferT<cVector2> cInputBuffer;

	static const unsigned int MAX_POSITIONS = 1024;

	cHeightMapQueryEffect();

	virtual ~cHeightMapQueryEffect();

	virtual void Release();

	bool Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );

	void DoTheThing( ID3D11DeviceContext* pContext, float mapWidth, float mapDepth, float mapHeightScale, ID3D11ShaderResourceView* pMapSRV, std::vector<cVector2>& positions, std::vector<float>& outputHeights );

	ID3DX11EffectTechnique* Tech;

	ID3DX11EffectScalarVariable* VarMapWidth;
	ID3DX11EffectScalarVariable* VarMapDepth;
	ID3DX11EffectScalarVariable* VarHeightScale;
	ID3DX11EffectUnorderedAccessViewVariable* VarHeights;
	ID3DX11EffectShaderResourceVariable* VarPositions;
	ID3DX11EffectShaderResourceVariable* VarCoarseMapSRV;

	cOutputBuffer mOutputHeights;
	cInputBuffer mInputPositions;

private:
	void DoEEET( ID3D11DeviceContext* pContext, std::vector<cVector2>& positions, std::vector<float>& outputHeights );
};

#endif  //  __cHeightMapQueryEffect_HG_20130411__