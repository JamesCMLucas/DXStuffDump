#include "cRenderableTexture.h"


cRenderableTexture::cRenderableTexture()
	: mWidth(0)
	, mHeight(0)
	, mColourMapFormat(DXGI_FORMAT_UNKNOWN)
	, mColourMapSRV(0)
	, mUAV(0)
	, mColourMapRTV(0)
	, mDepthMapSRV(0)
	, mDepthMapDSV(0)
	, mbHasInitialized(false)
{

}

cRenderableTexture::~cRenderableTexture()
{
	this->Release();
}

void cRenderableTexture::Release()
{
	if ( mColourMapSRV )
	{
		mColourMapSRV->Release();
		mColourMapSRV = 0;
	}
	if ( mUAV )
	{
		mUAV->Release();
		mUAV = 0;
	}
	if ( mColourMapRTV )
	{
		mColourMapRTV->Release();
		mColourMapRTV = 0;
	}
	if ( mDepthMapSRV )
	{
		mDepthMapSRV->Release();
		mDepthMapSRV = 0;
	}
	if ( mDepthMapDSV )
	{
		mDepthMapDSV->Release();
		mDepthMapDSV = 0;
	}
	mbHasInitialized = false;
	mColourMapFormat = DXGI_FORMAT_UNKNOWN;
	mWidth = 0;
	mHeight = 0;
}

bool cRenderableTexture::Init( ID3D11Device* pDevice, UINT width, UINT height, bool hasColourMap, DXGI_FORMAT colourFormat )
{
	if ( mbHasInitialized )
	{
		return false;
	}

	ZeroMemory( &mViewport, sizeof( D3D11_VIEWPORT ) );
	mWidth = width;
	mHeight = height;

	mColourMapFormat = colourFormat;
	mbHasColourMap = hasColourMap;

	if ( !BuildDepthMap( pDevice ) )
	{
		return false;
	}

	if ( mbHasColourMap )
	{
		if ( !BuildColourMap( pDevice ) )
		{
			this->Release();
			return false;
		}
	}

	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;

	mViewport.Width = static_cast<FLOAT>( width );
	mViewport.Height = static_cast<FLOAT>( height );

	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	mbHasInitialized = true;
	return true;
}

bool cRenderableTexture::Resize( ID3D11Device* pDevice, UINT width, UINT height )
{
	if ( !mbHasInitialized )
	{
		return false;
	}

	mbHasInitialized = false;

	mWidth = width;
	mHeight = height;

	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = static_cast<FLOAT>( width );
	mViewport.Height = static_cast<FLOAT>( height );
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;


	if ( !BuildDepthMap( pDevice ) )
	{
		return false;
	}

	if ( mbHasColourMap )
	{
		if ( !BuildColourMap( pDevice ) )
		{
			this->Release();
			return false;
		}
	}

	mbHasInitialized = true;
	return true;
}

void cRenderableTexture::Begin( ID3D11DeviceContext* pContext, const float* clearColourRGBA )
{
	ID3D11RenderTargetView* renderTargets[1] = { mColourMapRTV };

	pContext->OMSetRenderTargets( 1, renderTargets, mDepthMapDSV );
	pContext->RSSetViewports( 1, &mViewport );

	// only clear if we actually created a color map.
	if( mColourMapRTV )
	{
		pContext->ClearRenderTargetView( mColourMapRTV, clearColourRGBA );
	}

	pContext->ClearDepthStencilView( mDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0 );

}

void cRenderableTexture::Finish( ID3D11DeviceContext* pContext )
{
	// After we have drawn to the color map, have the hardware generate
	// the lower mipmap levels.

	if( mColourMapSRV )
	{
		pContext->GenerateMips( mColourMapSRV );
	}
}

bool cRenderableTexture::BuildDepthMap( ID3D11Device* pDevice )
{
	if ( mDepthMapSRV )
	{
		mDepthMapSRV->Release();
		mDepthMapSRV = 0;
	}
	if ( mDepthMapDSV )
	{
		mDepthMapDSV->Release();
		mDepthMapDSV = 0;
	}

	HRESULT hr = S_OK;
	ID3D11Texture2D* depthMap = 0;
	D3D11_TEXTURE2D_DESC texDesc;

	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	hr = pDevice->CreateTexture2D( &texDesc, 0, &depthMap );
	if ( FAILED(hr) )
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;

	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = pDevice->CreateDepthStencilView( depthMap,&dsvDesc, &mDepthMapDSV );
	if ( FAILED(hr) )
	{
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = pDevice->CreateShaderResourceView( depthMap, &srvDesc, &mDepthMapSRV );
	if ( FAILED(hr) )
	{
		return false;
	}

	// View saves a reference to the texture so we can
	// release our reference.
	depthMap->Release();
	return true;
}

bool cRenderableTexture::BuildColourMap( ID3D11Device* pDevice )
{
	if ( mColourMapSRV )
	{
		mColourMapSRV->Release();
		mColourMapSRV = 0;
	}
	if ( mColourMapRTV )
	{
		mColourMapRTV->Release();
		mColourMapRTV = 0;
	}

	HRESULT hr = S_OK;

	ID3D11Texture2D* colorMap = 0;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory( &texDesc, sizeof(texDesc) );

	texDesc.Width     = mWidth;
	texDesc.Height    = mHeight;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 1;
	texDesc.Format    = mColourMapFormat;
	texDesc.SampleDesc.Count   = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage          = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags      = D3D11_BIND_RENDER_TARGET |
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags      = D3D11_RESOURCE_MISC_GENERATE_MIPS;


	hr = pDevice->CreateTexture2D( &texDesc, 0, &colorMap );
	if ( FAILED(hr) )
	{
		return false;
	}
	// Null description means to create a view to all mipmap levels
	// using the format the texture was created with.

	hr = pDevice->CreateRenderTargetView( colorMap, 0, &mColourMapRTV );
	if ( FAILED(hr) )
	{
		return false;
	}

	hr = pDevice->CreateShaderResourceView( colorMap, 0, &mColourMapSRV );
	if ( FAILED(hr) )
	{
		return false;
	}

	hr = pDevice->CreateUnorderedAccessView( colorMap, 0, &mUAV );
	if ( FAILED(hr) )
	{
		return false;
	}

	// View saves a reference to the texture so we can
	// release our reference.
	colorMap->Release();
	colorMap = 0;
	return true;
}