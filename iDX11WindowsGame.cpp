#include "iDX11WindowsGame.h"
#include "DXBase.h"

iDX11WindowsGame::iDX11WindowsGame()
	: mbHasInitialized(false)
{
	mClearColour[0] = 1.f;
	mClearColour[1] = 1.f;
	mClearColour[2] = 1.f;
	mClearColour[3] = 1.f;

	mBlendingFactors[0] = 0.f;
	mBlendingFactors[1] = 0.f;
	mBlendingFactors[2] = 0.f;
	mBlendingFactors[3] = 0.f;

	mDriverType		= D3D_DRIVER_TYPE_NULL;
	mFeatureLevel	= D3D_FEATURE_LEVEL_11_0;

	mSwapChain					= NULL;
	mRenderTargetView			= NULL;
	mDepthStencilBuffer			= NULL;
	mDepthEnabledStencilState	= NULL;
	mDepthDisabledStencilState	= NULL;
	mDepthStencilView			= NULL;
	mRasterState				= NULL;
	mAlphaEndabledBlendState	= NULL;
	mAlphaDisabledBlendState	= NULL;
}

iDX11WindowsGame::~iDX11WindowsGame()
{
	this->Release();
}

void iDX11WindowsGame::Release()
{
	// can't close while in fullscreen mode, so make sure we're not
	mSwapChain->SetFullscreenState( false, NULL );

	if ( mDevice )
	{
		mDevice->Release();
		mDevice = NULL;
	}
	if ( mContext )
	{
		mContext->Release();
		mContext = NULL;
	}
	if ( mSwapChain )
	{
		mSwapChain->Release();
		mSwapChain = NULL;
	}
	if ( mRenderTargetView )
	{
		mRenderTargetView->Release();
		mRenderTargetView = NULL;
	}
	if ( mDepthStencilView )
	{
		mDepthStencilView->Release();
		mDepthStencilView = NULL;
	}
	if ( mDepthStencilBuffer )
	{
		mDepthStencilBuffer->Release();
		mDepthStencilBuffer = NULL;
	}
	if ( mDepthEnabledStencilState )
	{
		mDepthEnabledStencilState->Release();
		mDepthEnabledStencilState = NULL;
	}
	if ( mDepthDisabledStencilState )
	{
		mDepthDisabledStencilState->Release();
		mDepthDisabledStencilState = NULL;
	}
	if ( mRasterState )
	{
		mRasterState->Release();
		mRasterState = NULL;
	}
	if ( mAlphaEndabledBlendState )
	{
		mAlphaEndabledBlendState->Release();
		mAlphaEndabledBlendState = NULL;
	}
	if ( mAlphaDisabledBlendState )
	{
		mAlphaDisabledBlendState->Release();
		mAlphaDisabledBlendState = NULL;
	}
}

bool iDX11WindowsGame::GameInit( HWND hWnd, const cGameSetupInfo& setup )
{
	if ( mbHasInitialized )
	{
		return false;
	}

	/// TODO : MAKE THIS AN OPTION
	mbVSync = false;

	HRESULT hr = S_OK;

	mHWND = hWnd;

	UINT createDeviceFlags = 0;
#ifndef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = setup.GetWindowWidth();
	sd.BufferDesc.Height = setup.GetWindowHeight();
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	if ( mbVSync )
	{
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
	} 
	else
	{
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 1;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;//!setup.bFullScreen;
	//sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;  // allow full-screen switching

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		mDriverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain( NULL, mDriverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &mSwapChain, &mDevice, &mFeatureLevel, &mContext );
		if( SUCCEEDED( hr ) )
		{
			break;
		}
	}

	if( FAILED( hr ) )
	{
		return false;
	}

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = mSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	if( FAILED( hr ) )
	{
		return false;
	}

	hr = mDevice->CreateRenderTargetView( pBackBuffer, NULL, &mRenderTargetView );
	pBackBuffer->Release();
	pBackBuffer = 0;
	if( FAILED( hr ) )
	{
		return false;
	}

	mContext->OMSetRenderTargets( 1, &mRenderTargetView, NULL );


	D3D11_TEXTURE2D_DESC			depthBufferDesc;

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = static_cast<UINT>( setup.GetWindowWidth() );
	depthBufferDesc.Height = static_cast<UINT>( setup.GetWindowHeight() );
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	
	// Create the texture for the depth buffer using the filled out description.
	hr = mDevice->CreateTexture2D( &depthBufferDesc, NULL, &mDepthStencilBuffer );
	if ( FAILED(hr) )
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC		depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC	depthStencilViewDesc;


	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


	// Create the depth stencil state.
	hr = mDevice->CreateDepthStencilState( &depthStencilDesc, &mDepthEnabledStencilState );
	if( FAILED(hr) )
	{
		return false;
	}

	// Set the depth stencil state.
	mContext->OMSetDepthStencilState( mDepthEnabledStencilState, 1 );


	// Make the depth disabled state
	depthStencilDesc.DepthEnable = false;
	hr = mDevice->CreateDepthStencilState( &depthStencilDesc, &mDepthDisabledStencilState );
	if( FAILED(hr) )
	{
		return false;
	}



	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	hr = mDevice->CreateDepthStencilView( mDepthStencilBuffer, &depthStencilViewDesc, &mDepthStencilView);
	if( FAILED(hr) )
	{
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	mContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);




	D3D11_RASTERIZER_DESC			rasterDesc;
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;//true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	hr = mDevice->CreateRasterizerState( &rasterDesc, &mRasterState );
	if( FAILED(hr) )
	{
		return false;
	}

	// Now set the rasterizer state.
	mContext->RSSetState( mRasterState );


	// Setup the viewport
	mViewport.Width = static_cast<float>( setup.GetWindowWidth() );
	mViewport.Height = static_cast<float>( setup.GetWindowHeight() );
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mContext->RSSetViewports( 1, &mViewport );


	// Set up the blend state
	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory( &blendStateDesc, sizeof(D3D11_BLEND_DESC) );

	// enable blend state
	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// create the blend state
	hr = mDevice->CreateBlendState( &blendStateDesc, &mAlphaEndabledBlendState );
	if ( FAILED(hr) )
	{
		return false;
	}
	float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	

	// change the description for an alpha disabled blend state
	blendStateDesc.RenderTarget[0].BlendEnable = FALSE;
	// create the blend state
	hr = mDevice->CreateBlendState( &blendStateDesc, &mAlphaDisabledBlendState );
	if ( FAILED(hr) )
	{
		return false;
	}
	// initial blend state isn't alpha enabled
	mContext->OMSetBlendState( mAlphaEndabledBlendState, blendFactor, 0xffffffff );
	mbHasInitialized = true;


	return true;
}

void iDX11WindowsGame::GameResize()
{
	HRESULT result = S_OK;

	UINT width = GetGameWindowWidth();
	UINT height = GetGameWindowHeight();

	mContext->OMSetRenderTargets( 0, 0, 0 );
	
	DXRelease( mRenderTargetView );
	DXRelease( mDepthStencilView );
	DXRelease( mDepthStencilBuffer );

	result = mSwapChain->ResizeBuffers( 0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0 );
	if ( FAILED( result ) )
	{
		bool shit = true;
		// error handling needed!
	}

	ID3D11Texture2D* pBackBuffer;
	result = mSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&pBackBuffer );
	if ( FAILED( result ) )
	{
		bool shit = true;
		// error handling needed!
	}

	result = mDevice->CreateRenderTargetView( pBackBuffer, NULL, &mRenderTargetView );
	if ( FAILED( result ) )
	{
		bool shit = true;
		// error handling needed!
	}
	DXRelease( pBackBuffer );

	// depth buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width     = width;
	depthStencilDesc.Height    = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;
	result = mDevice->CreateTexture2D( &depthStencilDesc, 0, &mDepthStencilBuffer );
	if ( FAILED(result) )
	{
		bool shit = true;
		// error handling needed!
	}
	result = mDevice->CreateDepthStencilView( mDepthStencilBuffer, 0, &mDepthStencilView );
	if ( FAILED(result) )
	{
		bool shit = true;
		// error handling needed!
	}

	mContext->OMSetRenderTargets( 1, &mRenderTargetView, mDepthStencilView );

	mViewport.Width = static_cast<float>( width );
	mViewport.Height = static_cast<float>( height );

	mContext->RSSetViewports( 1, &mViewport );
}

void iDX11WindowsGame::SetClearColour( float red, float green, float blue, float alpha )
{
	mClearColour[0] = red;
	mClearColour[1] = green;
	mClearColour[2] = blue;
	mClearColour[3] = alpha;
}

void iDX11WindowsGame::SetAlphaBlendFactors( float red, float green, float blue, float alpha )
{
	mBlendingFactors[0] = red;
	mBlendingFactors[1] = green;
	mBlendingFactors[2] = blue;
	mBlendingFactors[3] = alpha;
}

bool iDX11WindowsGame::ChangeRasterizerState( eCullMode cullMode, bool bWireFrame, bool bAntialiasedLine, bool bMultisampleEnable )
{
	D3D11_RASTERIZER_DESC RSDesc;
	memset( &RSDesc, 0, sizeof(D3D11_RASTERIZER_DESC) );
	RSDesc.FillMode = D3D11_FILL_SOLID;
	RSDesc.AntialiasedLineEnable = FALSE;	
	RSDesc.MultisampleEnable = FALSE;

	switch ( cullMode )
	{
	case E_CULL_FRONT: 
		RSDesc.CullMode = D3D11_CULL_FRONT; 
		break;
	case E_CULL_NONE:
		RSDesc.CullMode = D3D11_CULL_NONE;
		break;
	case E_CULL_BACK:	
	default:
		RSDesc.CullMode = D3D11_CULL_BACK;	
		break;
	}

	if ( bWireFrame )
	{
		RSDesc.FillMode = D3D11_FILL_WIREFRAME;
	}
	if ( bAntialiasedLine )
	{
		RSDesc.AntialiasedLineEnable = TRUE;
	}
	if ( bMultisampleEnable )
	{
		RSDesc.MultisampleEnable = TRUE;
	}

	mRasterState->Release();
	mRasterState = 0;

	HRESULT hr = mDevice->CreateRasterizerState( &RSDesc, &(mRasterState) );
	if( FAILED( hr ) )
	{
		return false;
	}

	mContext->RSSetState( mRasterState );
	return true;
}

void iDX11WindowsGame::BeginBackBufferRendering( bool bAlphaEnabled, bool bDepthEnabled )
{
	SetRasterState();
	SetRenderTargetBackBuffer();
	SetViewPort();
	if ( bAlphaEnabled )
	{
		SetAlphaEnabledBlendState();
	}
	else
	{
		SetAlphaDisabledBlendState();
	}
	if ( bDepthEnabled )
	{
		SetDepthEnabledStencilState();
	}
	else
	{
		SetDepthDisabledStencilState();
	}

	ClearDepthAndStencilView();
	ClearRenderTargetView();
}