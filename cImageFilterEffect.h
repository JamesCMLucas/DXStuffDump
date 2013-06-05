#ifndef __JL_cImageFilterEffect_HG_20130403__
#define __JL_cImageFilterEffect_HG_20130403__

#include "iEffect.h"

class cImageFilterEffect : public iEffect
{
	typedef iEffect Base;
public:

	cImageFilterEffect();

	virtual ~cImageFilterEffect();

	/// Releases stuff...ready for reuse or re initialization
	virtual void Release();

	virtual void SetSigma( float sigma );

	/// call Release first if re-initializing or resizing is needed
	virtual bool Init( ID3D11Device* pDevice, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT );

	/// make sure to set the back buffer as the rendering target first!!!
	void FilterItBaby( ID3D11DeviceContext* pContext, ID3D11ShaderResourceView* srv, ID3D11UnorderedAccessView* uav, int cycles );

	ID3D11ShaderResourceView* GetOutputSRV();

protected:
	float mBlurWeights[9];

	UINT mWidth;
	UINT mHeight;
	DXGI_FORMAT mFormat;

	ID3D11ShaderResourceView* mBlurredOutputTexSRV;
	ID3D11UnorderedAccessView* mBlurredOutputTexUAV;

	ID3DX11EffectTechnique* mTechHorzBlur;
	ID3DX11EffectTechnique* mTechVertBlur;

	ID3DX11EffectScalarVariable* mVarWeights;
	ID3DX11EffectShaderResourceVariable* mVarInputMap;
	ID3DX11EffectUnorderedAccessViewVariable* mVarOutputMap;
};

#endif  //  __JL_cImageFilterEffect_HG_20130403__