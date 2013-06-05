#ifndef __JL_cShadowEffect_HG_20130319__
#define __JL_cShadowEffect_HG_20130319__

#include "iEffect.h"
#include "DXBase.h"
#include "sVert_PNTx.h"

class cShadowEffect : public iEffect
{
public:
	typedef iEffect Base;

	cShadowEffect();
	virtual ~cShadowEffect();

	virtual void Release();

	bool Init( ID3D11Device* pDevice );

	void SetTransform( const cMatrix4x4& worldViewProj );

	void SetInputLayout( ID3D11DeviceContext* pContext );

	void SetContext( ID3D11DeviceContext* pContext );

protected:
	ID3D11InputLayout* mInputLayout;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mVarWorldViewProj;

};

#endif  //  __JL_cShadowEffect_HG_20130319__