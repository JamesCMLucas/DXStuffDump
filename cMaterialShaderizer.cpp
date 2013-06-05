#include "cMaterialShaderizer.h"

#include "DXBase.h"


cMaterialShaderizer::cMaterialShaderizer()
{
	mEffectFileName = L"../Resources/Effects/MaterialShaderizer.fx";
	mMaterialPtr = 0;
}

cMaterialShaderizer::~cMaterialShaderizer()
{
	this->Release();
}

void cMaterialShaderizer::Release()
{
	DXRelease( mInputLayoutPNTxTn );
}

bool cMaterialShaderizer::Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	if ( !Base::LoadFXFile( mEffectFileName, pDevice ) )
	{
		return false;
	}
	mbInitialized = false;

	// get stuff
	if ( !GetTechniques() || !GetInputLayouts( pDevice ) || !GetVariables() )
	{
		return false;
	}

	// set up the default material - mimick them not being there
	if (!mDefaultMaterial.LoadDiffuseMap( pDevice, L"../Resources/Materials/white.png" ) ||
		!mDefaultMaterial.LoadDisplacementMap( pDevice, L"../Resources/Materials/black.png" ) ||
		!mDefaultMaterial.LoadNormalMap( pDevice, L"../Resources/Materials/blue.png" ) ||
		!mDefaultMaterial.LoadSpecularMap( pDevice, L"../Resources/Materials/white.png" ) ||
		!mDefaultMaterial.LoadOcclusionMap( pDevice, L"../Resources/Materials/white.png" )  )
	{
		return false;
	}

	mbInitialized = true;
	return true;
}

void cMaterialShaderizer::PushFrameVariables()
{
	mVarView->SetMatrix( ViewMatrix.m );
	mVarProjection->SetMatrix( ProjectionMatrix.m );
	mVarEyePosition->SetRawValue( &EyePosition.x, 0, sizeof(cVector3) );
	mVarTargetPosition->SetRawValue( &TargetPosition.x, 0, sizeof(cVector3) );

	PushLightAndShadowVariables();
}

void cMaterialShaderizer::PushObjectVariables()
{
	mVarWorld->SetMatrix( WorldMatrix.m );
	mVarWorldInvTranspose->SetMatrix( WorldInvTransposeMatrix.m );
	mVarWorldViewProj->SetMatrix( WorldViewProjectionMatrix.m );
	mVarShadowTransforms->AsMatrix()->SetMatrixArray( LightController.ShadowTransformArray[0].m, 0, LightController.mNumberOfLights );
	
	PushMaterialVariables();
}

void cMaterialShaderizer::SetMaterial( cMaterial* pMaterial )
{
	mMaterialPtr = pMaterial;  // assuming it's a full material for now!!
}

void cMaterialShaderizer::SetInputLayout( ID3D11DeviceContext* pContext )
{
	pContext->IASetInputLayout( mInputLayoutPNTxTn );
}

void cMaterialShaderizer::SetContext( ID3D11DeviceContext* pContext )
{
	PushLightAndShadowVariables();
	PushMaterialVariables();

	switch ( LightController.mNumberOfLights )
	{
	case 0:
		{
			mTechFullMaterial_L0->GetPassByIndex(0)->Apply( 0, pContext );
		}
		break;
	case 1:
		{
			mTechFullMaterial_L1->GetPassByIndex(0)->Apply( 0, pContext );
		}
		break;
	case 2:
		{
			mTechFullMaterial_L2->GetPassByIndex(0)->Apply( 0, pContext );
		}
		break;
	case 3:
		{
			mTechFullMaterial_L3->GetPassByIndex(0)->Apply( 0, pContext );
		}
		break;
	case 4:
		{
			mTechFullMaterial_L3->GetPassByIndex(0)->Apply( 0, pContext );
		}
		break;
	default:
		{
			mTechFullMaterial_L0->GetPassByIndex(0)->Apply( 0, pContext );
		}
		break;
	}
	
}

bool cMaterialShaderizer::GetTechniques()
{
	mTechFullMaterial_L0 = mEffect->GetTechniqueByName("TechFullMaterial_L0");
	mTechFullMaterial_L1 = mEffect->GetTechniqueByName("TechFullMaterial_L1");
	mTechFullMaterial_L2 = mEffect->GetTechniqueByName("TechFullMaterial_L2");
	mTechFullMaterial_L3 = mEffect->GetTechniqueByName("TechFullMaterial_L3");
	mTechFullMaterial_L4 = mEffect->GetTechniqueByName("TechFullMaterial_L4");

	if ( !mTechFullMaterial_L0->IsValid() || !mTechFullMaterial_L1->IsValid()
		|| !mTechFullMaterial_L2->IsValid() || !mTechFullMaterial_L3->IsValid()
		|| !mTechFullMaterial_L4->IsValid() )
	{
		return false;
	}

	return true;
}

bool cMaterialShaderizer::GetInputLayouts( ID3D11Device* pDevice )
{
	D3DX11_PASS_DESC passDesc;
	mTechFullMaterial_L0->GetPassByIndex(0)->GetDesc(&passDesc);
	
	HRESULT result = pDevice->CreateInputLayout( sVert_PNTxTn::GetElementDescPtr(), sVert_PNTxTn::NUM_ELEMENTS, 
		passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayoutPNTxTn );
	
	if ( FAILED( result ) )
	{
		this->Release();
		return false;
	}

	return true;
}

bool cMaterialShaderizer::GetVariables()
{
	// per-frame transform variables
	mVarView = mEffect->GetVariableByName("View")->AsMatrix();
	mVarProjection = mEffect->GetVariableByName("Projection")->AsMatrix();
	mVarEyePosition = mEffect->GetVariableByName("EyePosition");
	mVarTargetPosition = mEffect->GetVariableByName("TargetPosition");
	// check
	if ( !mVarView->IsValid() || !mVarProjection->IsValid()
		|| !mVarEyePosition->IsValid() || !mVarTargetPosition->IsValid() )
	{
		return false;
	}


	// per-object transform variables
	mVarWorld = mEffect->GetVariableByName("World")->AsMatrix();
	mVarWorldInvTranspose = mEffect->GetVariableByName("WorldInvTranspose")->AsMatrix();
	mVarWorldViewProj = mEffect->GetVariableByName("WorldViewProj")->AsMatrix();
	mVarTexTransform = mEffect->GetVariableByName("TexTransform")->AsMatrix();

	// per-object material descriptions
	mVarMaterialColour = mEffect->GetVariableByName("MaterialColour");
	mVarMapFactors = mEffect->GetVariableByName("MapFactors");

	// check
	if ( !mVarWorld->IsValid() || !mVarWorldInvTranspose->IsValid()
		|| !mVarWorldViewProj->IsValid() || !mVarTexTransform->IsValid()
		|| !mVarMaterialColour->IsValid() || !mVarMapFactors->IsValid() )
	{
		return false;
	}


	// texture variables
	mVarDiffuseMap = mEffect->GetVariableByName("DiffuseMap")->AsShaderResource();
	mVarDisplacementMap = mEffect->GetVariableByName("DisplacementMap")->AsShaderResource();
	mVarNormalMap = mEffect->GetVariableByName("NormalMap")->AsShaderResource();
	mVarOcclusionMap = mEffect->GetVariableByName("OcclusionMap")->AsShaderResource();
	mVarSpecularMap = mEffect->GetVariableByName("SpecularMap")->AsShaderResource();
	mVarShadowMapArray = mEffect->GetVariableByName("ShadowMapArray")->AsShaderResource();
	// check
	if ( !mVarDiffuseMap->IsValid() || !mVarDisplacementMap->IsValid()
		|| !mVarNormalMap->IsValid() || !mVarOcclusionMap->IsValid()
		|| !mVarSpecularMap->IsValid() || !mVarShadowMapArray->IsValid() )
	{
		return false;
	}


	// lighting variables
	mVarLights = mEffect->GetVariableByName("Lights");
	mVarShadowTransforms = mEffect->GetVariableByName("ShadowTransforms");
	mVarNumberOfLights = mEffect->GetVariableByName("NumberOfLights")->AsScalar();
	// check
	if ( !mVarLights->IsValid() || !mVarShadowTransforms->IsValid()
		|| !mVarNumberOfLights->IsValid() )
	{
		return false;
	}

	// horray!
	return true;
}

void cMaterialShaderizer::PushLightAndShadowVariables()
{
	LightController.Update();
	mVarNumberOfLights->SetInt( LightController.mNumberOfLights );
	mVarLights->SetRawValue( LightController.LightArray, 0, sizeof(sGeneralLight)*LightController.mNumberOfLights );
	mVarShadowMapArray->SetResourceArray( LightController.ShadowMapSRVArray, 0, LightController.mNumberOfLights );
}

void cMaterialShaderizer::PushMaterialVariables()
{
	if ( !mMaterialPtr )
	{
		// set the default material
		mVarMaterialColour->SetRawValue( &( mDefaultMaterial.GetMaterialColour() ), 0, sizeof(sColourMaterial) );
		mVarDiffuseMap->SetResource( mDefaultMaterial.GetDiffuseSRVPtr() );
		mVarDisplacementMap->SetResource( mDefaultMaterial.GetDisplacementSRVPtr() );
		mVarNormalMap->SetResource( mDefaultMaterial.GetNormalSRVPtr() );
		mVarOcclusionMap->SetResource( mDefaultMaterial.GetOcclusionSRVPtr() );
		mVarSpecularMap->SetResource( mDefaultMaterial.GetSpecularSRVPtr() );

		cVector4 factors(0.f,0.f,0.f,0.f);
		mVarMapFactors->SetRawValue( &factors.x, 0, sizeof(cVector4) );

		return;
	}

	// global colour
	mVarMaterialColour->SetRawValue( &( mMaterialPtr->GetMaterialColour() ), 0, sizeof(sColourMaterial) );
	
	// diffuse map
	if ( mMaterialPtr->HasDiffuseMap() )
	{
		mVarDiffuseMap->SetResource( mMaterialPtr->GetDiffuseSRVPtr() );
	}
	else
	{
		mVarDiffuseMap->SetResource( mDefaultMaterial.GetDiffuseSRVPtr() );
	}

	// displacement map
	if ( mMaterialPtr->HasDisplacementeMap() )
	{
		mVarDisplacementMap->SetResource( mMaterialPtr->GetDisplacementSRVPtr() );
		cVector4 factors = mMaterialPtr->GetMapFactors();
		mVarMapFactors->SetRawValue( &factors.x, 0, sizeof(cVector4) );
	}
	else
	{
		mVarDisplacementMap->SetResource( mDefaultMaterial.GetDisplacementSRVPtr() );
		cVector4 factors(0.f,0.f,0.f,0.f);
		mVarMapFactors->SetRawValue( &factors.x, 0, sizeof(cVector4) );
	}

	// normal map
	if ( mMaterialPtr->HasNormaleMap() )
	{
		mVarNormalMap->SetResource( mMaterialPtr->GetNormalSRVPtr() );
	}
	else
	{
		mVarNormalMap->SetResource( mDefaultMaterial.GetNormalSRVPtr() );
	}

	// ambient occlusion map
	if ( mMaterialPtr->HasOcclusionMap() )
	{
		mVarOcclusionMap->SetResource( mMaterialPtr->GetOcclusionSRVPtr() );
	}
	else
	{
		mVarOcclusionMap->SetResource( mDefaultMaterial.GetOcclusionSRVPtr() );
	}

	// specular map
	if ( mMaterialPtr->HasSpecularMap() )
	{
		mVarSpecularMap->SetResource( mMaterialPtr->GetSpecularSRVPtr() );
	}
	else
	{
		mVarSpecularMap->SetResource( mDefaultMaterial.GetSpecularSRVPtr() );
	}

}

