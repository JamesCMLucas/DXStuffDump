#ifndef __JL_cSkyBoxEffect_HG_20130402__
#define __JL_cSkyBoxEffect_HG_20130402__

#include "iEffect.h"
#include "cRenderBuffersT.h"
#include "sVert_P.h"

class cSkyBoxEffect : public iEffect
{
	typedef iEffect Base;
	typedef cRenderBuffersT<sVert_P> cSkyBuffers;
public:
	cSkyBoxEffect();

	virtual ~cSkyBoxEffect();

	virtual bool Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	virtual void BeginFrame( ID3D11DeviceContext* pContext );
	virtual void FinishFrame( ID3D11DeviceContext* pContext );
	virtual void ManualRender( ID3D11DeviceContext* pContext, const cMatrix4x4& worldViewProj, ID3D11ShaderResourceView* srv );
	
private:
	cSkyBuffers mBuffers;
	ID3D11InputLayout* mLayout;
	ID3DX11EffectTechnique* mTechnique;
	ID3DX11EffectShaderResourceVariable* mSRV;
	ID3DX11EffectVariable* mTransform;
};

#endif  //  __JL_cSkyBoxEffect_HG_20130402__