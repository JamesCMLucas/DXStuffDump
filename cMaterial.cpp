#include "cMaterial.h"
#include <sstream>


const std::wstring cMaterial::BURNED_WOOD = L"burned wood";
const std::wstring cMaterial::BURNED_WOOD2 = L"burned wood2";
const std::wstring cMaterial::CRACKED_GROUND = L"cracked ground";
const std::wstring cMaterial::CRACKED_STONE = L"cracked stone";
const std::wstring cMaterial::CRUST = L"crust";
const std::wstring cMaterial::PEBBLES = L"pebbles";
const std::wstring cMaterial::PUTRID_WOOD = L"putrid wood";
const std::wstring cMaterial::STONE = L"stone";
const std::wstring cMaterial::GRASS = L"grass";

cMaterial::cMaterial()
{
	mColourMaterial.Ambient = nColour::WHITE;
	mColourMaterial.Diffuse = nColour::WHITE;
	mColourMaterial.Specular = nColour::WHITE;
	mMapFactors.Zeroes();
}

cMaterial::~cMaterial()
{
	mDiffuseMap.Release();
	mDisplacementMap.Release();
	mNormalMap.Release();
	mOcclusionMap.Release();
	mSpecularMap.Release();
}

ID3D11ShaderResourceView* cMaterial::GetSpecularSRVPtr() const
{
	return mSpecularMap.mSRV;
}

ID3D11ShaderResourceView* cMaterial::GetOcclusionSRVPtr() const
{
	return mOcclusionMap.mSRV;
}

ID3D11ShaderResourceView* cMaterial::GetNormalSRVPtr() const
{
	return mNormalMap.mSRV;
}

ID3D11ShaderResourceView* cMaterial::GetDisplacementSRVPtr() const
{
	return mDisplacementMap.mSRV;
}

ID3D11ShaderResourceView* cMaterial::GetDiffuseSRVPtr() const
{
	return mDiffuseMap.mSRV;
}

bool cMaterial::HasSpecularMap()
{
	return mSpecularMap.IsLoaded();
}

bool cMaterial::HasOcclusionMap()
{
	return mOcclusionMap.IsLoaded();
}

bool cMaterial::HasNormaleMap()
{
	return mNormalMap.IsLoaded();
}

bool cMaterial::HasDisplacementeMap()
{
	return mDisplacementMap.IsLoaded();
}

bool cMaterial::HasDiffuseMap()
{
	return mDiffuseMap.IsLoaded();
}

void cMaterial::SetSpecularPower( float power )
{
	mColourMaterial.Specular.a = power;
}

void cMaterial::SetSpecularColour( float r, float g, float b, float power )
{
	mColourMaterial.Specular = cColour4( r, g, b, power );
}

void cMaterial::SetSpecularColour( float r, float g, float b )
{
	mColourMaterial.Specular.r = r;
	mColourMaterial.Specular.g = g;
	mColourMaterial.Specular.b = b;
}

void cMaterial::SetSpecularColour( const cColour3& specularColour )
{
	mColourMaterial.Specular.r = specularColour.r;
	mColourMaterial.Specular.g = specularColour.g;
	mColourMaterial.Specular.b = specularColour.b;
}

void cMaterial::SetAmbientColour( float r, float g, float b, float a )
{
	mColourMaterial.Ambient = cColour4( r, g, b, a );
}

void cMaterial::SetAmbientColour( const cColour4& ambientColour )
{
	mColourMaterial.Diffuse = ambientColour;
}

void cMaterial::SetDiffuseColour( float r, float g, float b, float a )
{
	mColourMaterial.Diffuse = cColour4( r, g, b, a );
}

void cMaterial::SetDiffuseColour( const cColour4& diffuseColour )
{
	mColourMaterial.Diffuse = diffuseColour;
}

void cMaterial::SetDisplacementMapFactor( float factor )
{
	mMapFactors.x = factor;
	if ( factor < 0.f )
	{
		mMapFactors.x = 0.f;
	}
}

bool cMaterial::LoadSpecularMap( ID3D11Device* pDevice, std::wstring fileName )
{
	mSpecularMap.Release();
	return mSpecularMap.LoadFromFile( fileName, pDevice );
}

bool cMaterial::LoadOcclusionMap( ID3D11Device* pDevice, std::wstring fileName )
{
	mOcclusionMap.Release();
	return mOcclusionMap.LoadFromFile( fileName, pDevice );
}

bool cMaterial::LoadNormalMap( ID3D11Device* pDevice, std::wstring fileName )
{
	mNormalMap.Release();
	return mNormalMap.LoadFromFile( fileName, pDevice );
}

bool cMaterial::LoadDisplacementMap( ID3D11Device* pDevice, std::wstring fileName )
{
	mDisplacementMap.Release();
	return mDisplacementMap.LoadFromFile( fileName, pDevice );
}

bool cMaterial::LoadDiffuseMap( ID3D11Device* pDevice, std::wstring fileName )
{
	mDiffuseMap.Release();
	return mDiffuseMap.LoadFromFile( fileName, pDevice );
}

bool cMaterial::LoadByName( ID3D11Device* pDevice, std::wstring matName )
{
	std::wstringstream wss;
	wss << L"../Resources/Materials/" << matName << L"/" << matName;
	
	std::wstring strBegin = wss.str();

	// load the diffuse map
	wss << L"_COLOR.jpg";
	if ( !mDiffuseMap.LoadFromFile( wss.str(), pDevice ) )
	{
		return false;
	}
	wss.str( std::wstring(L"") );

	// load the displacement map
	wss << strBegin << L"_DISP.jpg";
	if ( !mDisplacementMap.LoadFromFile( wss.str(), pDevice ) )
	{
		return false;
	}
	wss.str( std::wstring(L"") );

	// load the normal map
	wss << strBegin << L"_NRM.jpg";
	if ( !mNormalMap.LoadFromFile( wss.str(), pDevice ) )
	{
		return false;
	}
	wss.str( std::wstring(L"") );

	// load the occlusion map
	wss << strBegin << L"_OCC.jpg";
	if ( !mOcclusionMap.LoadFromFile( wss.str(), pDevice ) )
	{
		return false;
	}
	wss.str( std::wstring(L"") );

	// load the specular map
	wss << strBegin << L"_SPEC.jpg";
	if ( !mSpecularMap.LoadFromFile( wss.str(), pDevice ) )
	{
		return false;
	}
	wss.str( std::wstring(L"") );

	return true;
}

const sColourMaterial& cMaterial::GetMaterialColour()
{
	return mColourMaterial;
}

float cMaterial::GetDisplacementMapFactor() const
{
	return mMapFactors.x;
}

const cVector4& cMaterial::GetMapFactors()
{
	return mMapFactors;
}
