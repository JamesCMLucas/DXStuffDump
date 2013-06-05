#include "cShadowEffect.h"


cShadowEffect::cShadowEffect()
{
	mEffectFileName = L"../Resources/Effects/ShadowMap.fx";
}

cShadowEffect::~cShadowEffect()
{
	this->Release();
}

void cShadowEffect::Release()
{
	Base::Release();
	DXRelease(mInputLayout);
}

bool cShadowEffect::Init( ID3D11Device* pDevice )
{
	if ( !Base::LoadFXFile( mEffectFileName, pDevice ) )
	{
		return false;
	}
	mbInitialized = false;

	// get the technique
	mTech = mEffect->GetTechniqueByName("TechBuildShadowMap");
	// make the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HRESULT result = pDevice->CreateInputLayout( sVert_PNTx::GetElementDescPtr(), sVert_PNTx::NUM_ELEMENTS, 
		passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout );
	if ( FAILED( result ) )
	{
		this->Release();
		return false;
	}

	// get the transformation matrix variable
	mVarWorldViewProj = mEffect->GetVariableByName("WorldViewProj")->AsMatrix();
	if ( !mVarWorldViewProj->IsValid() )
	{
		this->Release();
		return false;
	}



	mbInitialized = true;
	return true;
}

void cShadowEffect::SetTransform( const cMatrix4x4& worldViewProj )
{
	mVarWorldViewProj->SetMatrix( worldViewProj.m );
}

void cShadowEffect::SetInputLayout( ID3D11DeviceContext* pContext )
{
	pContext->IASetInputLayout( mInputLayout );
}

void cShadowEffect::SetContext( ID3D11DeviceContext* pContext )
{
	mTech->GetPassByIndex(0)->Apply( 0, pContext );
}
