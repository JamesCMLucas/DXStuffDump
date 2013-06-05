#ifndef __JL_cMaterial_HG_20130427__
#define __JL_cMaterial_HG_20130427__

#include "cTexture.h"
#include "Colours.h"

class cMaterial
{
public:
	static const std::wstring BURNED_WOOD;
	static const std::wstring BURNED_WOOD2;
	static const std::wstring CRACKED_GROUND;
	static const std::wstring CRACKED_STONE;
	static const std::wstring CRUST;
	static const std::wstring PEBBLES;
	static const std::wstring PUTRID_WOOD;
	static const std::wstring STONE;
	static const std::wstring GRASS;

	cMaterial();

	virtual ~cMaterial();

	bool LoadByName( ID3D11Device* pDevice, std::wstring matName );
	
	bool LoadDiffuseMap(  ID3D11Device* pDevice, std::wstring fileName );

	bool LoadDisplacementMap(  ID3D11Device* pDevice, std::wstring fileName );

	bool LoadNormalMap(  ID3D11Device* pDevice, std::wstring fileName );

	bool LoadOcclusionMap(  ID3D11Device* pDevice, std::wstring fileName );

	bool LoadSpecularMap(  ID3D11Device* pDevice, std::wstring fileName );

	void SetDiffuseColour( const cColour4& diffuseColour );

	void SetDiffuseColour( float r, float g, float b, float a );

	void SetAmbientColour( const cColour4& ambientColour );

	void SetAmbientColour( float r, float g, float b, float a );

	void SetSpecularColour( const cColour3& specularColour );
	
	void SetSpecularColour( float r, float g, float b );

	void SetSpecularColour( float r, float g, float b, float power );

	void SetSpecularPower( float power );

	void SetDisplacementMapFactor( float factor );

	bool HasDiffuseMap();

	bool HasDisplacementeMap();

	bool HasNormaleMap();

	bool HasOcclusionMap();

	bool HasSpecularMap();

	ID3D11ShaderResourceView* GetDiffuseSRVPtr() const;

	ID3D11ShaderResourceView* GetDisplacementSRVPtr() const;

	ID3D11ShaderResourceView* GetNormalSRVPtr() const;

	ID3D11ShaderResourceView* GetOcclusionSRVPtr() const;

	ID3D11ShaderResourceView* GetSpecularSRVPtr() const;

	const sColourMaterial& GetMaterialColour();

	float GetDisplacementMapFactor() const;

	const cVector4& GetMapFactors();

protected:
	
	sColourMaterial mColourMaterial;

	// x = displacement map factor
	cVector4 mMapFactors;
	

	cTexture mDiffuseMap;
	cTexture mDisplacementMap;
	cTexture mNormalMap;
	cTexture mOcclusionMap;
	cTexture mSpecularMap;
};

#endif  //  __JL_cMaterial_HG_20130427__