#include "cShadowMap.h"


cShadowMap::cShadowMap()
	: mbHasInitialized(false)
	, mDepthMapDSV(0)
	, mDepthMapSRV(0)
{

}

cShadowMap::~cShadowMap()
{
	this->Release();
}

void cShadowMap::Release()
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
	mWidth = 0;
	mHeight = 0;
	mbHasInitialized = false;
}

bool cShadowMap::Initialize( ID3D11Device* pDevice, unsigned int width, unsigned int height )
{
	if ( mbHasInitialized )
	{
		return false;
	}
	mWidth = width;
	mHeight = height;

	mViewport.TopLeftX = 0.f;
	mViewport.TopLeftY = 0.f;
	mViewport.Width = static_cast<float>(mWidth);
	mViewport.Height = static_cast<float>(mHeight);
	mViewport.MinDepth = 0.f;
	mViewport.MaxDepth = 1.f;

	// Use typeless format because the DSV is going to interpret
	// the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going to interpret
	// the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width     = mWidth;
	texDesc.Height    = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format    = DXGI_FORMAT_R32_TYPELESS;//DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count   = 1;  
	texDesc.SampleDesc.Quality = 0;  
	texDesc.Usage          = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0; 
	texDesc.MiscFlags      = 0;

	ID3D11Texture2D* depthMap = 0;
	HRESULT result = pDevice->CreateTexture2D( &texDesc, 0, &depthMap );
	if ( FAILED( result ) )
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	result = pDevice->CreateDepthStencilView( depthMap, &dsvDesc, &mDepthMapDSV );
	if ( FAILED( result ) )
	{
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;//DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	result = pDevice->CreateShaderResourceView( depthMap, &srvDesc, &mDepthMapSRV );
	if ( FAILED( result ) )
	{
		return false;
	}

	// done
	depthMap->Release();

	mbHasInitialized = true;
	return true;
}

bool cShadowMap::Resize( ID3D11Device* pDevice, unsigned int width, unsigned int height )
{
	this->Release();
	return this->Initialize( pDevice, width, height );
}

void cShadowMap::Begin( ID3D11DeviceContext* pContext )
{
	pContext->RSSetViewports( 1, &mViewport );

	// set null render target...we're only drawing to the depth buffer
	// setting null will disable colour writing
	pContext->OMSetRenderTargets( 0, 0, mDepthMapDSV );
	pContext->ClearDepthStencilView( mDepthMapDSV, D3D11_CLEAR_DEPTH, 1.f, 0 );
}

void cShadowMap::Finish( ID3D11DeviceContext* pContext )
{
	pContext->RSSetViewports( 0, 0 );
	pContext->OMSetRenderTargets( 0, 0, 0 );
}
