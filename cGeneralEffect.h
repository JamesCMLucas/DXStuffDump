#ifndef __cGeneralEffect_HG_20130412__
#define __cGeneralEffect_HG_20130412__

#include "iEffect.h"
#include "Vertices.h"
#include "DXBase.h"

class cGeneralEffect : public iEffect
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
	sColourMaterial				ColourMaterial;

	cGeneralEffect();
	virtual ~cGeneralEffect();

	virtual void Release();

	bool Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	
	void SetFrameVariables();
	void SetObjectVariables();

	void SetPerFrameJunk( const cMatrix4x4& view, const cMatrix4x4& proj, const cVector3& eyePos, const cVector3& targetPos, 
		std::vector<sDirectionalLight>* pVecDirectionalLights, std::vector<sPointLight>* pVecPointLights, std::vector<sSpotLight>* pVecSpotLights );

	void SetPerObjectJunk( const cMatrix4x4& world, const cMatrix4x4& worldViewProj, const sColourMaterial& colourMaterial );

	void SetPerObjectJunk( const cMatrix4x4& world, const cMatrix4x4& worldViewProj, const cMatrix4x4& shadowTransform, const sColourMaterial& colourMaterial );

	void SetContext( ID3D11DeviceContext* pContext );
	void SetContextShadow( ID3D11DeviceContext* pContext );
	void SetInputLayout( ID3D11DeviceContext* pContext );
	void SetShadowMap( ID3D11ShaderResourceView* pSRV );

protected:
	bool GetVariables();
	
	// technique
	ID3DX11EffectTechnique* mTechGeneralPNTx;
	ID3DX11EffectTechnique* mTechShadowPNTx;
	ID3DX11EffectTechnique* mTechTruck;

	// input layout
	ID3D11InputLayout* mInputLayoutPNTx;

	// texture stuff
	ID3DX11EffectShaderResourceVariable* mVarDiffuseMapTexture;
	
	// per-frame stuff
	ID3DX11EffectMatrixVariable* mVarView;
	ID3DX11EffectMatrixVariable* mVarProjection;
	ID3DX11EffectVariable* mVarEyePosition;
	ID3DX11EffectVariable* mVarTargetPosition;
	ID3DX11EffectShaderResourceVariable* mVarShadowMap;
	ID3DX11EffectShaderResourceVariable* mVarShadowMapArray;

	// per-object stuff
	ID3DX11EffectMatrixVariable* mVarWorld;
	ID3DX11EffectMatrixVariable* mVarWorldInvTranspose;
	ID3DX11EffectMatrixVariable* mVarWorldViewProj;
	ID3DX11EffectMatrixVariable* mVarShadowTransform;
	ID3DX11EffectMatrixVariable* mVarTexTransform;
	ID3DX11EffectVariable* mVarColourMaterial;

	// lighting stuff
	ID3DX11EffectVariable* mVarLights;
	ID3DX11EffectScalarVariable* mVarNumberOfLights;

	// 
	ID3DX11EffectVariable* mVarDirectionalLights;
	ID3DX11EffectVariable* mVarPointLights;
	ID3DX11EffectVariable* mVarSpotLights;
	ID3DX11EffectScalarVariable* mVarNumberOfDirectionalLights;
	ID3DX11EffectScalarVariable* mVarNumberOfPointLights;
	ID3DX11EffectScalarVariable* mVarNumberOfSpotLights;

};

#endif  //  __cGeneralEffect_HG_20130412__