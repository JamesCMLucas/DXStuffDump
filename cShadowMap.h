#ifndef __JL_cShadowMap_HG_20130317__
#define __JL_cShadowMap_HG_20130317__

#include <D3DX11.h>
#include <cMatrix4x4.h>

class cShadowMap
{
public:
	static const unsigned int SHADOW_MAP_SIZE = 2048;

	cShadowMap();
	virtual ~cShadowMap();

	inline bool HasInitialized() const { return mbHasInitialized; }
	inline UINT GetWidth() const { return mWidth; }
	inline UINT GetHeight() const { return mHeight; }
	inline ID3D11ShaderResourceView* const* GetDepthMapSRV() { return &mDepthMapSRV; }
	inline ID3D11DepthStencilView* const* GetDepthMapDSV() { return &mDepthMapDSV; }
	
	virtual void Release();
	virtual bool Initialize( ID3D11Device* pDevice, unsigned int width, unsigned int height );
	
	virtual bool Resize( ID3D11Device* pDevice, unsigned int width, unsigned int height );
	virtual void Begin( ID3D11DeviceContext* pContext );
	virtual void Finish( ID3D11DeviceContext* pContext );
	
	cMatrix4x4 ViewMatrix;
	cMatrix4x4 PerspectiveMatrix;
protected:
	UINT mWidth;
	UINT mHeight;
	ID3D11ShaderResourceView* mDepthMapSRV;
	ID3D11DepthStencilView* mDepthMapDSV;
	D3D11_VIEWPORT mViewport;
private:
	cShadowMap( const cShadowMap& rhs );
	cShadowMap& operator=( const cShadowMap& rhs );

	bool mbHasInitialized;
};

#endif