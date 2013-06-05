#include "cGeneralEffect.h"


cGeneralEffect::cGeneralEffect()
{
	mEffectFileName = L"../Resources/Effects/General.fx";
}

cGeneralEffect::~cGeneralEffect()
{
	this->Release();
}

void cGeneralEffect::Release()
{
	Base::Release();
	DXRelease( mInputLayoutPNTx );
}

bool cGeneralEffect::Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	if ( !Base::LoadFXFile( mEffectFileName, pDevice ) )
	{
		return false;
	}
	mbInitialized = false;

	// get the techniques
	mTechGeneralPNTx = mEffect->GetTechniqueByName("TechGeneralPNTx");
	mTechShadowPNTx = mEffect->GetTechniqueByName("TechShadowPNTx");
	mTechTruck = mEffect->GetTechniqueByName("TechTruck");

	if ( !mTechGeneralPNTx->IsValid() || !mTechShadowPNTx->IsValid() )
	{
		return false;
	}

	// make the input layout
	D3DX11_PASS_DESC passDesc;
	mTechGeneralPNTx->GetPassByIndex(0)->GetDesc(&passDesc);
	HRESULT result = pDevice->CreateInputLayout( sVert_PNTx::GetElementDescPtr(), sVert_PNTx::NUM_ELEMENTS, 
		passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayoutPNTx );
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

void cGeneralEffect::SetFrameVariables()
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

void cGeneralEffect::SetObjectVariables()
{
	mVarWorld->SetMatrix( WorldMatrix.m );
	mVarWorldInvTranspose->SetMatrix( WorldInvTransposeMatrix.m );
	mVarWorldViewProj->SetMatrix( WorldViewProjectionMatrix.m );
	mVarColourMaterial->SetRawValue( &ColourMaterial, 0, sizeof(sColourMaterial) );
	mVarShadowTransform->SetMatrix( ShadowTransformMatrix.m );
}

void cGeneralEffect::SetPerFrameJunk( const cMatrix4x4& view, const cMatrix4x4& proj, const cVector3& eyePos, const cVector3& targetPos, std::vector<sDirectionalLight>* pVecDirectionalLights, std::vector<sPointLight>* pVecPointLights, std::vector<sSpotLight>* pVecSpotLights )
{
	cMatrix4x4 v = cMatrix4x4::Transpose( view );
	cMatrix4x4 p = cMatrix4x4::Transpose( proj );

	mVarView->SetMatrix( v.m );
	mVarProjection->SetMatrix( p.m );
	mVarEyePosition->SetRawValue( &eyePos.x, 0, sizeof(cVector3) );
	mVarTargetPosition->SetRawValue( &targetPos.x, 0, sizeof(cVector3) );

	if ( pVecDirectionalLights )
	{
		unsigned int numLights = pVecDirectionalLights->size();
		HRESULT result = mVarDirectionalLights->SetRawValue( &(*pVecDirectionalLights)[0], 0, numLights*sizeof(sDirectionalLight) );
		result = mVarNumberOfDirectionalLights->SetInt( numLights );
	}
	else
	{
		mVarNumberOfDirectionalLights->SetInt( 0 );
	}

	if ( pVecPointLights )
	{
		unsigned int numLights = pVecPointLights->size();
		mVarPointLights->SetRawValue( &(*pVecPointLights)[0], 0, numLights*sizeof(sPointLight) );
		mVarNumberOfPointLights->SetInt( numLights );
	}
	else
	{
		mVarNumberOfPointLights->SetInt( 0 );
	}

	if ( pVecSpotLights )
	{
		unsigned int numLights = pVecSpotLights->size();
		mVarSpotLights->SetRawValue( &(*pVecSpotLights)[0], 0, numLights*sizeof(sSpotLight) );
		mVarNumberOfSpotLights->SetInt( numLights );
	}
	else
	{
		mVarNumberOfSpotLights->SetInt( 0 );
	}
}

void cGeneralEffect::SetPerObjectJunk( const cMatrix4x4& world, const cMatrix4x4& worldViewProj, const sColourMaterial& colourMaterial )
{
	cMatrix4x4 wInvTr = cMatrix4x4::InverseTranspose( world );

	mVarWorld->SetMatrixTranspose( world.m );
	mVarWorldInvTranspose->SetMatrixTranspose( wInvTr.m );
	mVarWorldViewProj->SetMatrixTranspose( worldViewProj.m );
	mVarColourMaterial->SetRawValue( &colourMaterial, 0, sizeof(sColourMaterial) );
}

void cGeneralEffect::SetPerObjectJunk( const cMatrix4x4& world, const cMatrix4x4& worldViewProj, const cMatrix4x4& shadowTransform, const sColourMaterial& colourMaterial )
{
	cMatrix4x4 wInvTr = cMatrix4x4::InverseTranspose( world );

	mVarWorld->SetMatrixTranspose( world.m );
	mVarWorldInvTranspose->SetMatrixTranspose( wInvTr.m );
	mVarWorldViewProj->SetMatrix( worldViewProj.m );
	mVarShadowTransform->SetMatrix( shadowTransform.m );
	mVarColourMaterial->SetRawValue( &colourMaterial, 0, sizeof(sColourMaterial) );
}

void cGeneralEffect::SetContext( ID3D11DeviceContext* pContext )
{
	mTechGeneralPNTx->GetPassByIndex(0)->Apply( 0, pContext );
}

void cGeneralEffect::SetContextShadow( ID3D11DeviceContext* pContext )
{
	mTechShadowPNTx->GetPassByIndex(0)->Apply( 0, pContext );
}

void cGeneralEffect::SetInputLayout( ID3D11DeviceContext* pContext )
{
	pContext->IASetInputLayout( mInputLayoutPNTx );
}

bool cGeneralEffect::GetVariables()
{
	// get the per frame garbage
	mVarView = mEffect->GetVariableByName("View")->AsMatrix();
	mVarProjection = mEffect->GetVariableByName("Projection")->AsMatrix();
	mVarEyePosition = mEffect->GetVariableByName("EyePosition");
	mVarTargetPosition = mEffect->GetVariableByName("TargetPosition");
	mVarShadowMap = mEffect->GetVariableByName("ShadowMap")->AsShaderResource();
	mVarShadowMapArray = mEffect->GetVariableByName("ShadowMapArray")->AsShaderResource();

	// check it
	if ( !mVarView->IsValid() || !mVarProjection->IsValid() )
	{
		return false;
	}
	if ( !mVarEyePosition->IsValid() || !mVarTargetPosition->IsValid() || !mVarShadowMap->IsValid() || !mVarShadowMapArray->IsValid() )
	{
		return false;
	}


	// get the per object junk
	mVarWorld = mEffect->GetVariableByName("World")->AsMatrix();
	mVarWorldInvTranspose = mEffect->GetVariableByName("WorldInvTranspose")->AsMatrix();
	mVarWorldViewProj = mEffect->GetVariableByName("WorldViewProj")->AsMatrix();
	mVarTexTransform = mEffect->GetVariableByName("TexTransform")->AsMatrix();
	mVarShadowTransform = mEffect->GetVariableByName("ShadowTransform")->AsMatrix();
	mVarColourMaterial = mEffect->GetVariableByName("Material");
	// check it
	if ( !mVarWorld->IsValid() || !mVarWorldInvTranspose->IsValid() 
		|| !mVarWorldViewProj->IsValid() || !mVarTexTransform->IsValid()
		|| !mVarShadowTransform->IsValid() || !mVarColourMaterial->IsValid() )
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

	mVarDirectionalLights = mEffect->GetVariableByName("DirectionalLights");
	mVarSpotLights = mEffect->GetVariableByName("SpotLights");
	mVarPointLights = mEffect->GetVariableByName("PointLights");
	mVarNumberOfDirectionalLights = mEffect->GetVariableByName("NumberOfDirectionalLights")->AsScalar();
	mVarNumberOfSpotLights = mEffect->GetVariableByName("NumberOfSpotLights")->AsScalar();
	mVarNumberOfPointLights = mEffect->GetVariableByName("NumberOfPointLights")->AsScalar();
	// check it
	if ( !mVarWorld->IsValid() || !mVarWorldInvTranspose->IsValid() 
		|| !mVarWorldViewProj->IsValid() || !mVarTexTransform->IsValid()
		|| !mVarColourMaterial->IsValid() )
	{
		return false;
	}

	return true;
}

void cGeneralEffect::SetShadowMap( ID3D11ShaderResourceView* pSRV )
{
	mVarShadowMap->SetResource( pSRV );
	ID3D11ShaderResourceView* apsrv[] = { pSRV };
	mVarShadowMapArray->SetResourceArray( apsrv, 0, 1 );
}