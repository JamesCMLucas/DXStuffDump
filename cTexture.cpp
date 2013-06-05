#include "cTexture.h"
#include <D3DX11tex.h>
#include <winerror.h>


cTexture::cTexture()
	: mSRV(0)
	, mbIsLoaded(false)
{
	mType = E_TEXTURE_TYPE_UNKNOWN;
}

cTexture::~cTexture()
{
	this->Release();
}

void cTexture::Release()
{
	if ( mSRV )
	{
		mSRV->Release();
		mSRV = 0;
	}
	mFileName = L"";
	mbIsLoaded = false;
}

bool cTexture::LoadFromFile( std::wstring fileName, ID3D11Device* pDevice )
{
	if ( mbIsLoaded )
	{
		return false;
	}

	HRESULT result = D3DX11CreateShaderResourceViewFromFile( pDevice, fileName.c_str(), NULL, NULL, &mSRV, NULL );

	if ( FAILED( result ) )
	{
		return false;
	}

	mFileName = fileName;
	mbIsLoaded = true;
	return true;
}

