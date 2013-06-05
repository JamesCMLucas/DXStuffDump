#ifndef __cBumpedAndShadowedFX_HG_20130422__
#define __cBumpedAndShadowedFX_HG_20130422__

#include "iEffect.h"
#include "Vertices.h"
#include "DXBase.h"

class cBumpedAndShadowedFX : public iEffect
{
public:
	typedef iEffect Base;

	static const unsigned int MAX_LIGHTS = 4;

	// per frame
	std::vector<sGeneralLight>	Lights;
	cMatrix4x4					ViewMatrix;
	cMatrix4x4					ProjectionMatrix;
	cVector3					EyePosition;
	cVector3					TargetPosition;

	// per object
	cMatrix4x4					WorldMatrix;
	cMatrix4x4					WorldInvTransposeMatrix;
	cMatrix4x4					WorldViewProjectionMatrix;
	cMatrix4x4					ShadowTransformMatrix;

	cBumpedAndShadowedFX();
	virtual ~cBumpedAndShadowedFX();

	virtual void Release();

	bool Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );

	void SetFrameVariables();
	void SetObjectVariables();

	void SetContext( ID3D11DeviceContext* pContext );
	void SetInputLayout( ID3D11DeviceContext* pContext );

	void SetShadowMap( ID3D11ShaderResourceView* pSRV );
	void SetDiffuseMap( ID3D11ShaderResourceView* pSRV );
	void SetDisplacementMap( ID3D11ShaderResourceView* pSRV, float displacementFctor );
	void SetNormalMap( ID3D11ShaderResourceView* pSRV );
	void SetOcclusionMap( ID3D11ShaderResourceView* pSRV );
	void SetSpecularMap( ID3D11ShaderResourceView* pSRV );
protected:
	bool GetVariables();

	// technique
	ID3DX11EffectTechnique* mTech;

	// input layout
	ID3D11InputLayout* mInputLayoutPNTxTnBt;

	// per-frame stuff
	ID3DX11EffectMatrixVariable* mVarView;
	ID3DX11EffectMatrixVariable* mVarProjection;
	ID3DX11EffectVariable* mVarEyePosition;
	ID3DX11EffectVariable* mVarTargetPosition;
	ID3DX11EffectShaderResourceVariable* mVarShadowMap;
	ID3DX11EffectShaderResourceVariable* mVarShadowMapArray;

	// per object stuff
	ID3DX11EffectMatrixVariable* mVarWorld;
	ID3DX11EffectMatrixVariable* mVarWorldInvTranspose;
	ID3DX11EffectMatrixVariable* mVarWorldViewProj;
	ID3DX11EffectMatrixVariable* mVarShadowTransform;
	ID3DX11EffectMatrixVariable* mVarTexTransform;
	

	ID3DX11EffectShaderResourceVariable* mVarDiffuseColourMap;
	ID3DX11EffectShaderResourceVariable* mVarDisplacementMap;
	ID3DX11EffectShaderResourceVariable* mVarNormalMap;
	ID3DX11EffectShaderResourceVariable* mVarOcclusionMap;
	ID3DX11EffectShaderResourceVariable* mVarSpecularMap;
	ID3DX11EffectVariable* mVarMapFactors;


	// lighting stuff
	ID3DX11EffectVariable* mVarLights;
	ID3DX11EffectScalarVariable* mVarNumberOfLights;
};

#endif  //  __cBumpedAndShadowedFX_HG_20130422__