#include "cImageFilterEffect.h"


cImageFilterEffect::cImageFilterEffect() : mBlurredOutputTexSRV(0)
	, mBlurredOutputTexUAV(0)
{
	SetSigma(1.f);
	mEffectFileName = L"../Resources/Effects/ImageFilter.fx";
}

cImageFilterEffect::~cImageFilterEffect()
{
	this->Release();
}

void cImageFilterEffect::Release()
{
	Base::Release();
	if ( mBlurredOutputTexSRV )
	{
		mBlurredOutputTexSRV->Release();
		mBlurredOutputTexSRV = 0;
	}
	if ( mBlurredOutputTexUAV )
	{
		mBlurredOutputTexUAV->Release();
		mBlurredOutputTexUAV = 0;
	}
	mbInitialized = false;
}

void cImageFilterEffect::SetSigma( float sigma )
{
	float d = 2.0f*sigma*sigma;

	float sum = 0.0f;
	for(int i = 0; i < 8; ++i)
	{
		float x = (float)i;
		mBlurWeights[i] = expf(-x*x/d);

		sum += mBlurWeights[i];
	}

	// Divide by the sum so all the weights add up to 1.0.
	for(int i = 0; i < 8; ++i)
	{
		mBlurWeights[i] /= sum;
	}
}

bool cImageFilterEffect::Init( ID3D11Device* pDevice, UINT width, UINT height, DXGI_FORMAT format /*= DXGI_FORMAT_R32G32B32A32_FLOAT */ )
{
	if ( mbInitialized )
	{
		Release();
	}

	if ( !Base::LoadFXFile( mEffectFileName, pDevice ) )
	{
		return false;
	}

	// get the variables
	mTechHorzBlur = mEffect->GetTechniqueByName("HorzBlur");
	mTechVertBlur = mEffect->GetTechniqueByName("VertBlur");

	mVarWeights = mEffect->GetVariableByName("gWeights")->AsScalar();
	mVarInputMap = mEffect->GetVariableByName("gInput")->AsShaderResource();
	mVarOutputMap = mEffect->GetVariableByName("gOutput")->AsUnorderedAccessView();


	mWidth = width;
	mHeight = height;
	mFormat = format;

	// Note, compressed formats cannot be used for UAV.  We get error like:
	// ERROR: ID3D11Device::CreateTexture2D: The format (0x4d, BC3_UNORM) 
	// cannot be bound as an UnorderedAccessView, or cast to a format that
	// could be bound as an UnorderedAccessView.  Therefore this format 
	// does not support D3D11_BIND_UNORDERED_ACCESS.

	D3D11_TEXTURE2D_DESC blurredTexDesc;
	blurredTexDesc.Width     = width;
	blurredTexDesc.Height    = height;
	blurredTexDesc.MipLevels = 1;
	blurredTexDesc.ArraySize = 1;
	blurredTexDesc.Format    = format;
	blurredTexDesc.SampleDesc.Count   = 1;
	blurredTexDesc.SampleDesc.Quality = 0;
	blurredTexDesc.Usage     = D3D11_USAGE_DEFAULT;
	blurredTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	blurredTexDesc.CPUAccessFlags = 0;
	blurredTexDesc.MiscFlags      = 0;

	ID3D11Texture2D* blurredTex = 0;
	if ( FAILED( pDevice->CreateTexture2D( &blurredTexDesc, 0, &blurredTex ) ) )
	{
		Release();
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	if ( FAILED( pDevice->CreateShaderResourceView( blurredTex, &srvDesc, &mBlurredOutputTexSRV ) ) )
	{
		Release();
		return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	if ( FAILED( pDevice->CreateUnorderedAccessView( blurredTex, &uavDesc, &mBlurredOutputTexUAV ) ) )
	{
		Release();
		return false;
	}

	// Views save a reference to the texture so we can release our reference
	blurredTex->Release();
	blurredTex = 0;

	mbInitialized = true;
	return true;
}

void cImageFilterEffect::FilterItBaby( ID3D11DeviceContext* pContext, ID3D11ShaderResourceView* srv, ID3D11UnorderedAccessView* uav, int cycles )
{
	//eugh...
	pContext->VSSetShader( 0, 0, 0 );
	pContext->PSSetShader( 0, 0, 0 );

	

	for ( int idxFilterCount = 0; idxFilterCount < cycles; idxFilterCount++ )
	{
		// HORIZONTAL blur pass.
		D3DX11_TECHNIQUE_DESC techDesc;
		mTechHorzBlur->GetDesc( &techDesc );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			mVarInputMap->SetResource( srv );
			mVarOutputMap->SetUnorderedAccessView( mBlurredOutputTexUAV );
			mTechHorzBlur->GetPassByIndex(p)->Apply( 0, pContext );

			// How many groups do we need to dispatch to cover a row of pixels, where each
			// group covers 256 pixels (the 256 is defined in the ComputeShader).
			UINT numGroupsX = static_cast<UINT>( ceilf(mWidth / 256.0f) );
			pContext->Dispatch( numGroupsX, mHeight, 1 );

			// Unbind the input texture from the CS for good housekeeping.
			ID3D11ShaderResourceView* nullSRV[1] = { 0 };
			pContext->CSSetShaderResources( 0, 1, nullSRV );

			// Unbind output from compute shader (we are going to use this output as an input in the next pass, 
			// and a resource cannot be both an output and input at the same time.
			ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
			pContext->CSSetUnorderedAccessViews( 0, 1, nullUAV, 0 );

			// VERTICAL blur pass.
			mTechVertBlur->GetDesc( &techDesc );
			for( UINT p = 0; p < techDesc.Passes; ++p )
			{
				mVarInputMap->SetResource(mBlurredOutputTexSRV);
				mVarOutputMap->SetUnorderedAccessView( uav );
				mTechVertBlur->GetPassByIndex(p)->Apply( 0, pContext );

				// How many groups do we need to dispatch to cover a column of pixels, where each
				// group covers 256 pixels  (the 256 is defined in the ComputeShader).
				UINT numGroupsY = static_cast<UINT>( ceilf(mHeight / 256.0f) );
				pContext->Dispatch( mWidth, numGroupsY, 1 );
			}

			pContext->CSSetShaderResources( 0, 1, nullSRV );
			pContext->CSSetUnorderedAccessViews( 0, 1, nullUAV, 0 );
		}
	}

	// Disable compute shader.
	pContext->CSSetShader( 0, 0, 0 );
}

ID3D11ShaderResourceView* cImageFilterEffect::GetOutputSRV()
{
	return mBlurredOutputTexSRV;
}
