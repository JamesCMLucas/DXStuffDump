#ifndef __cMaterialShaderizer_HG_20130422__
#define __cMaterialShaderizer_HG_20130422__

#include "iEffect.h"
#include "Vertices.h"
#include "DXBase.h"
#include "cShadowMap.h"
#include "cMaterial.h"

class cMaterialShaderizer : public iEffect
{
public:
	typedef iEffect Base;

	static const unsigned int MAX_LIGHTS = 4;

	class cLightController
	{
	public:
		cLightController()
		{
			mNumberOfLights = 0;
			for ( unsigned int idx = 0; idx < MAX_LIGHTS; idx++ )
			{
				LightArray[idx] = sGeneralLight();
				ShadowMapSRVArray[idx] = 0;
			}
		}
		
		sGeneralLight				LightArray[MAX_LIGHTS];
		ID3D11ShaderResourceView*	ShadowMapSRVArray[MAX_LIGHTS];
		cMatrix4x4					ShadowTransformArray[MAX_LIGHTS];

	private:
		void Update()
		{
			unsigned int idxLight = 0;
			for ( idxLight = 0; idxLight < MAX_LIGHTS; idxLight++ )
			{
				if ( LightArray[idxLight].IsOff() )
				{
					break;
				}
				else
				{
					if ( LightArray[idxLight].IsShadowed() && !ShadowMapSRVArray[idxLight] )
					{
						LightArray[idxLight].Shadowed = 0;
					}
				}
			}
			mNumberOfLights = (int)idxLight;
		}
		int mNumberOfLights;

		friend class cMaterialShaderizer;
	};

	

	// lights
	cLightController				LightController;
	
	// neccessary per-frame variables
	cMatrix4x4						ViewMatrix;
	cMatrix4x4						ProjectionMatrix;
	cVector3						EyePosition;
	cVector3						TargetPosition;

	// neccessary per-object variables
	cMatrix4x4						WorldMatrix;
	cMatrix4x4						WorldInvTransposeMatrix;
	cMatrix4x4						WorldViewProjectionMatrix;

	cMaterialShaderizer();
	virtual ~cMaterialShaderizer();

	virtual void Release();

	bool Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );


	void PushFrameVariables();

	void PushObjectVariables();

	// can be set to null - call before SetContext
	void SetMaterial( cMaterial* pMaterial );

	// first step
	void SetInputLayout( ID3D11DeviceContext* pContext );
	// last step before telling your buffers to draw
	void SetContext( ID3D11DeviceContext* pContext );

protected:
	bool GetTechniques();
	bool GetInputLayouts( ID3D11Device* pDevice );
	bool GetVariables();
	
	void PushLightAndShadowVariables();
	void PushMaterialVariables();

	cMaterial* mMaterialPtr;
	cMaterial mDefaultMaterial;

	// techniques
	ID3DX11EffectTechnique* mTechFullMaterial_L0;
	ID3DX11EffectTechnique* mTechFullMaterial_L1;
	ID3DX11EffectTechnique* mTechFullMaterial_L2;
	ID3DX11EffectTechnique* mTechFullMaterial_L3;
	ID3DX11EffectTechnique* mTechFullMaterial_L4;

	// input layout
	ID3D11InputLayout* mInputLayoutPNTxTn;

	// per-frame transform variables
	ID3DX11EffectMatrixVariable*			mVarView;
	ID3DX11EffectMatrixVariable*			mVarProjection;
	ID3DX11EffectVariable*					mVarEyePosition;
	ID3DX11EffectVariable*					mVarTargetPosition;

	// per-object transform variables
	ID3DX11EffectMatrixVariable*			mVarWorld;
	ID3DX11EffectMatrixVariable*			mVarWorldInvTranspose;
	ID3DX11EffectMatrixVariable*			mVarWorldViewProj;
	ID3DX11EffectMatrixVariable*			mVarTexTransform;  // not used...

	// per-object material descriptions
	ID3DX11EffectVariable*					mVarMaterialColour;
	ID3DX11EffectVariable*					mVarMapFactors;

	// texture variables
	ID3DX11EffectShaderResourceVariable*	mVarDiffuseMap;
	ID3DX11EffectShaderResourceVariable*	mVarDisplacementMap;
	ID3DX11EffectShaderResourceVariable*	mVarNormalMap;
	ID3DX11EffectShaderResourceVariable*	mVarOcclusionMap;
	ID3DX11EffectShaderResourceVariable*	mVarSpecularMap;
	ID3DX11EffectShaderResourceVariable*	mVarShadowMapArray;

	// lighting variables
	ID3DX11EffectVariable*					mVarLights;
	ID3DX11EffectVariable*					mVarShadowTransforms;
	ID3DX11EffectScalarVariable*			mVarNumberOfLights;  // maybe make this a uniform parameter in the pixel shader?
};

#endif  //  __cMaterialShaderizer_HG_20130422__