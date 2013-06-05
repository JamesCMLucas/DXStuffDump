#include "c2DSpriteEffect.h"


c2DSpriteEffect::c2DSpriteEffect() : mBuffer(E_BUFFER_TYPE_VERTEX)
	, mTechSprite(0)
	, mVarTex(0)
	, mLayout(0)
{
	mEffectFileName = L"../Resources/Effects/2DSpriteEffect.fx";
}

c2DSpriteEffect::~c2DSpriteEffect()
{
	this->Release();
}

void c2DSpriteEffect::Release()
{
	Base::Release();
	mTechSprite = 0;
	mVarTex = 0;
	if ( mLayout )
	{
		mLayout->Release();
		mLayout = 0;
	}
}

bool c2DSpriteEffect::Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	if ( mbInitialized )
	{
		return false;
	}

	if ( !mBuffer.Init( pDevice, 6 ) )
	{
		return false;
	}

	// load the fx file
	if ( !Base::LoadFXFile( mEffectFileName, pDevice ) )
	{
		return false;
	}

	// get the technique
	mTechSprite = mEffect->GetTechniqueByName("TechBasicSprite");

	// create the input layout
	D3DX11_PASS_DESC passDesc;
	mTechSprite->GetPassByIndex(0)->GetDesc(&passDesc);
	HRESULT result = pDevice->CreateInputLayout( SPRITE_EFFECT_2D_ELEMENT_DESC, 2, 
		passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mLayout );
	if ( FAILED( result ) )
	{
		Base::Release();
		return false;
	}

	// get the texture
	mVarTex = mEffect->GetVariableByName("SpriteTexture")->AsShaderResource();

	// dunzo
	mbInitialized = true;
	return true;
}

void c2DSpriteEffect::ManualRender( ID3D11DeviceContext* pContext, ID3D11ShaderResourceView* srv, const sRect2D& rect2D )
{
	mVarTex->SetResource( srv );

	sVertex* pVerts = mBuffer.BeginMapping( pContext );
	pVerts->Position = rect2D.Position;
	pVerts->HalfWidths = rect2D.HalfWidths;
	mBuffer.FinishMapping( pContext );

	pContext->IASetInputLayout( mLayout );
	pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	UINT stride = sizeof(sVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers( 0, 1, mBuffer.GetDXBufferConstPtr(), &stride, &offset );

	mTechSprite->GetPassByIndex(0)->Apply( 0, pContext );
	pContext->Draw( 1, 0 );
}
