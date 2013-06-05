#ifndef __JL_iDX11WindowsGame_HG_20130319__
#define __JL_iDX11WindowsGame_HG_20130319__

#include "iWindowsGame.h"
#include <D3DX11.h>

enum eCullMode
{
	E_CULL_BACK,
	E_CULL_FRONT,
	E_CULL_NONE
};

class iDX11WindowsGame : public iWindowsGame
{
public:
	iDX11WindowsGame();
	virtual ~iDX11WindowsGame();

	virtual void Release();
	virtual bool GameInit( HWND hWnd, const cGameSetupInfo& setup );
	virtual void GameResize();



	virtual void SetClearColour( float red, float green, float blue, float alpha );
	virtual void SetAlphaBlendFactors( float red, float green, float blue, float alpha );
	virtual bool ChangeRasterizerState( eCullMode cullMode, bool bWireFrame, bool bAntialiasedLine, bool bMultisampleEnable );
	

	/// Clears everything and prepares all states for backbuffer rendering
	virtual void BeginBackBufferRendering( bool bAlphaEnabled, bool bDepthEnabled );
	
	/// Semi useful inline functions
	inline void SetAlphaEnabledBlendState()
	{
		mContext->OMSetBlendState( mAlphaEndabledBlendState, mBlendingFactors, 0xffffffff );
	}
	inline void SetAlphaDisabledBlendState()
	{
		mContext->OMSetBlendState( mAlphaDisabledBlendState, mBlendingFactors, 0xffffffff );
	}
	inline void SetDepthEnabledStencilState()
	{
		mContext->OMSetDepthStencilState( mDepthEnabledStencilState, 1 );
	}
	inline void SetDepthDisabledStencilState()
	{
		mContext->OMSetDepthStencilState( mDepthDisabledStencilState, 1 );
	}
	inline void SetRenderTargetBackBuffer()
	{
		mContext->OMSetRenderTargets( 1, &mRenderTargetView, mDepthStencilView );
	}
	inline void SetViewPort()
	{
		mContext->RSSetViewports( 1, &mViewport );
	}
	inline void SetRasterState()
	{
		mContext->RSSetState( mRasterState );
	}
	inline void ClearState()
	{
		mContext->ClearState();
	}
	inline void ClearRenderTargetView()
	{
		mContext->ClearRenderTargetView( mRenderTargetView, mClearColour );
	}
	inline void ClearDepthView()
	{
		mContext->ClearDepthStencilView( mDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0 );
	}
	inline void ClearStencilView()
	{
		mContext->ClearDepthStencilView( mDepthStencilView, D3D11_CLEAR_STENCIL, 1.f, 0 );
	}
	inline void ClearDepthAndStencilView()
	{
		mContext->ClearDepthStencilView( mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0 );
	}
	inline void FinishRendering()
	{
		mSwapChain->Present( 0, 0 );
	}
protected:
	HWND mHWND;
	/// The clear colour
	float						mClearColour[4];
	/// The alpha blending factors
	float						mBlendingFactors[4];
	/// Have we initialized?
	bool mbHasInitialized;
	/// Synchronized frame rate?
	bool mbVSync;
	/// Viewport
	D3D11_VIEWPORT				mViewport;
	/// Driver type
	D3D_DRIVER_TYPE				mDriverType;
	/// Feature level
	D3D_FEATURE_LEVEL			mFeatureLevel;
	/// Device
	ID3D11Device*				mDevice;
	/// Context
	ID3D11DeviceContext*		mContext;
	/// Swap chain
	IDXGISwapChain*				mSwapChain;
	/// RTV
	ID3D11RenderTargetView*		mRenderTargetView;
	/// DSV
	ID3D11DepthStencilView*		mDepthStencilView;
	/// Tex - Depth stencil buffer
	ID3D11Texture2D*			mDepthStencilBuffer;
	/// Stencil state for depth testing
	ID3D11DepthStencilState*	mDepthEnabledStencilState;
	/// Stencil state for no depth testing
	ID3D11DepthStencilState*	mDepthDisabledStencilState;
	/// Rasterizer state
	ID3D11RasterizerState*		mRasterState;
	/// Blending State for alpha blending
	ID3D11BlendState*			mAlphaEndabledBlendState;
	/// Blending State for no alpha blending
	ID3D11BlendState*			mAlphaDisabledBlendState;

private:
	// disable assignment / inhabit / copy
	iDX11WindowsGame( const cGameWindow& ) {};
	iDX11WindowsGame& operator = ( const cGameWindow& ) {};
	iDX11WindowsGame( cGameWindow&& ) {}
	iDX11WindowsGame& operator = ( cGameWindow&& ) {};
};

#endif  //  __JL_iDX11WindowsGame_HG_20130319__