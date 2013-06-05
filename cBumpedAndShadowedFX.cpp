#include "cBumpedAndShadowedFX.h"
#include "sVert_PNTxTnBt.h"

cBumpedAndShadowedFX::cBumpedAndShadowedFX()
{
	mEffectFileName = L"../Resources/Effects/BumpMappedAndShadowed.fx";
}

cBumpedAndShadowedFX::~cBumpedAndShadowedFX()
{
	this->Release();
}

void cBumpedAndShadowedFX::Release()
{
	Base::Release();
	DXRelease( mInputLayoutPNTxTnBt );
}

bool cBumpedAndShadowedFX::Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	if ( !Base::LoadFXFile( mEffectFileName, pDevice ) )
	{
		return false;
	}
	mbInitialized = false;

	// get the techniques
	mTech = mEffect->GetTechniqueByName("Tech");
	
	if ( !mTech->IsValid() )
	{
		return false;
	}

	// make the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HRESULT result = pDevice->CreateInputLayout( sVert_PNTxTnBt::GetElementDescPtr(), sVert_PNTxTnBt::NUM_ELEMENTS, 
		passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayoutPNTxTnBt );
	if ( FAILED( result ) )
	{
		Base::Release();
		return false;
	}

	if ( !GetVariables() )
	{
		return false;
	}



	mbInitialized = true;
	return true;
}

void cBumpedAndShadowedFX::SetContext( ID3D11DeviceContext* pContext )
{
	mTech->GetPassByIndex(0)->Apply( 0, pContext );
}

void cBumpedAndShadowedFX::SetInputLayout( ID3D11DeviceContext* pContext )
{
	pContext->IASetInputLayout( mInputLayoutPNTxTnBt );
}

void cBumpedAndShadowedFX::SetShadowMap( ID3D11ShaderResourceView* pSRV )
{
	ID3D11ShaderResourceView* apsrv[] = { pSRV };
	mVarShadowMapArray->SetResourceArray( apsrv, 0, 1 );
}

bool cBumpedAndShadowedFX::GetVariables()
{
	// get the per frame garbage
	mVarView = mEffect->GetVariableByName("View")->AsMatrix();
	mVarProjection = mEffect->GetVariableByName("Projection")->AsMatrix();
	mVarEyePosition = mEffect->GetVariableByName("EyePosition");
	mVarTargetPosition = mEffect->GetVariableByName("TargetPosition");

	mVarShadowMapArray = mEffect->GetVariableByName("ShadowMapArray")->AsShaderResource();
	mVarDiffuseColourMap = mEffect->GetVariableByName("DiffuseColourMap")->AsShaderResource();
	mVarDisplacementMap = mEffect->GetVariableByName("DisplacementMap")->AsShaderResource();
	mVarNormalMap = mEffect->GetVariableByName("NormalMap")->AsShaderResource();
	mVarOcclusionMap = mEffect->GetVariableByName("OcclusionMap")->AsShaderResource();
	mVarSpecularMap = mEffect->GetVariableByName("SpecularMap")->AsShaderResource();
	mVarMapFactors = mEffect->GetVariableByName("MapFactors");

	// check it
	if ( !mVarView->IsValid() || !mVarProjection->IsValid() || !mVarMapFactors->IsValid() )
	{
		return false;
	}
	if ( !mVarEyePosition->IsValid() || !mVarTargetPosition->IsValid() 
		|| !mVarShadowMapArray->IsValid() || !mVarDiffuseColourMap->IsValid()
		|| !mVarDisplacementMap->IsValid() || !mVarNormalMap->IsValid()
		|| !mVarOcclusionMap->IsValid() || !mVarSpecularMap->IsValid() )
	{
		return false;
	}


	// get the per object junk
	mVarWorld = mEffect->GetVariableByName("World")->AsMatrix();
	mVarWorldInvTranspose = mEffect->GetVariableByName("WorldInvTranspose")->AsMatrix();
	mVarWorldViewProj = mEffect->GetVariableByName("WorldViewProj")->AsMatrix();
	mVarTexTransform = mEffect->GetVariableByName("TexTransform")->AsMatrix();
	mVarShadowTransform = mEffect->GetVariableByName("ShadowTransform")->AsMatrix();
	
	// check it
	if ( !mVarWorld->IsValid() || !mVarWorldInvTranspose->IsValid() 
		|| !mVarWorldViewProj->IsValid() || !mVarTexTransform->IsValid()
		|| !mVarShadowTransform->IsValid() )
	{
		return false;
	}

	// get the lighting stuff
	mVarLights = mEffect->GetVariableByName("Lights");
	mVarNumberOfLights = mEffect->GetVariableByName("NumberOfLights")->AsScalar();
	if ( !mVarLights->IsValid() || !mVarNumberOfLights->IsValid() )
	{
		return false;
	}

	return true;
}

void cBumpedAndShadowedFX::SetObjectVariables()
{
	mVarWorld->SetMatrix( WorldMatrix.m );
	mVarWorldInvTranspose->SetMatrix( WorldInvTransposeMatrix.m );
	mVarWorldViewProj->SetMatrix( WorldViewProjectionMatrix.m );
	mVarShadowTransform->SetMatrix( ShadowTransformMatrix.m );
}

void cBumpedAndShadowedFX::SetFrameVariables()
{
	// set the lights
	unsigned int numLights = Lights.size();
	if ( numLights > MAX_LIGHTS )
	{
		numLights = MAX_LIGHTS;
	}

	mVarNumberOfLights->SetInt( numLights );
	if ( numLights > 0 )
	{
		HRESULT result = mVarLights->SetRawValue( &Lights[0], 0, numLights*sizeof(sGeneralLight) );
		bool breaking = true;
	}

	mVarView->SetMatrix( ViewMatrix.m );
	mVarProjection->SetMatrix( ProjectionMatrix.m );
	mVarEyePosition->SetRawValue( &EyePosition.x, 0, sizeof(cVector3) );
	mVarTargetPosition->SetRawValue( &TargetPosition.x, 0, sizeof(cVector3) );
}

void cBumpedAndShadowedFX::SetDiffuseMap( ID3D11ShaderResourceView* pSRV )
{
	mVarDiffuseColourMap->SetResource( pSRV );
}

void cBumpedAndShadowedFX::SetDisplacementMap( ID3D11ShaderResourceView* pSRV, float displacementFctor )
{
	mVarDisplacementMap->SetResource( pSRV );
	mVarMapFactors->SetRawValue( &displacementFctor, 0, sizeof(float) );
}

void cBumpedAndShadowedFX::SetNormalMap( ID3D11ShaderResourceView* pSRV )
{
	mVarNormalMap->SetResource( pSRV );
}

void cBumpedAndShadowedFX::SetOcclusionMap( ID3D11ShaderResourceView* pSRV )
{
	mVarOcclusionMap->SetResource( pSRV );
}

void cBumpedAndShadowedFX::SetSpecularMap( ID3D11ShaderResourceView* pSRV )
{
	mVarSpecularMap->SetResource( pSRV );
}
