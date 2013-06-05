#ifndef __JL_cRenderableTexture_HG_20130317__
#define __JL_cRenderableTexture_HG_20130317__

#include <D3D11.h>

class cRenderableTexture
{
public:
	cRenderableTexture();
	virtual ~cRenderableTexture();

	inline UINT GetWidth() const { return mWidth; }
	inline UINT GetHeight() const { return mHeight; }
	inline bool HasColourMap() const { return mbHasColourMap; }
	inline bool HasInitialized() const { return mbHasInitialized; }

	inline ID3D11ShaderResourceView* GetColourMap() { return mColourMapSRV; }
	inline ID3D11ShaderResourceView* GetDepthMap() { return mDepthMapSRV; }
	inline ID3D11UnorderedAccessView* GetUAV() { return mUAV; }
	inline ID3D11ShaderResourceView* const* GetColourMapConstPtr() { return &mColourMapSRV; }
	inline ID3D11ShaderResourceView* const* GetDepthMapConstPtr() { return &mDepthMapSRV; }

	virtual void Release();

	virtual bool Init( ID3D11Device* pDevice, UINT width, UINT height, bool hasColourMap, DXGI_FORMAT colourFormat = DXGI_FORMAT_R32G32B32A32_FLOAT );
	virtual bool Resize( ID3D11Device* pDevice, UINT width, UINT height );

	virtual void Begin( ID3D11DeviceContext* pContext, const float* clearColourRGBA );
	virtual void Finish( ID3D11DeviceContext* pContext );

protected:
	

	UINT mWidth;
	UINT mHeight;

	bool mbHasColourMap;
	bool mbHasInitialized;
	DXGI_FORMAT mColourMapFormat;

	/// the tex for the shader
	ID3D11ShaderResourceView* mColourMapSRV;
	/// unordered access view
	ID3D11UnorderedAccessView* mUAV;
	/// to tex to render to
	ID3D11RenderTargetView* mColourMapRTV;
	/// depth map for the shader
	ID3D11ShaderResourceView* mDepthMapSRV;
	/// depth map to render to
	ID3D11DepthStencilView* mDepthMapDSV;

	D3D11_VIEWPORT mViewport;

private:
	bool BuildDepthMap( ID3D11Device* pDevice );
	bool BuildColourMap( ID3D11Device* pDevice );
};

#endif