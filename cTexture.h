#ifndef __JL_cTexture_HG_20130317__
#define __JL_cTexture_HG_20130317__

#include <D3D11.h>
#include <string>

enum eTextureType
{
	E_TEXTURE_TYPE_UNKNOWN		= 0,
	E_TEXTURE_TYPE_AMBIENT_MAP	= 1,
	E_TEXTURE_TYPE_DIFFUSE_MAP	= 2,
	E_TEXTURE_TYPE_SPECULAR_MAP = 4,
	E_TEXTURE_TYPE_EMISSIVE_MAP = 8,
	E_TEXTURE_TYPE_BUMP_MAP		= 16,
	E_TEXTURE_TYPE_HEIGHT_MAP	= 32
};

class cTexture
{
public:
	cTexture();
	virtual ~cTexture();

	inline bool IsLoaded() const { return mbIsLoaded; }
	inline std::wstring GetFileName() const { return mFileName; }
	inline ID3D11ShaderResourceView* GetColourMap() { return mSRV; }
	inline ID3D11ShaderResourceView* const* GetSRV() { return &mSRV; }
	inline eTextureType GetType() const { return mType; }
	inline void SetType( eTextureType type ) { mType = type; }

	virtual void Release();
	virtual bool LoadFromFile( std::wstring fileName, ID3D11Device* pDevice );

	ID3D11ShaderResourceView* mSRV;
protected:
	eTextureType mType;
	
	std::wstring mFileName;
private:
	bool mbIsLoaded;
};

#endif  //  __JL_cTexture_HG_20130317__