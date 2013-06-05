#ifndef __JL_c2DSpriteEffect_HG_20130403__
#define __JL_c2DSpriteEffect_HG_20130403__

#include "iEffect.h"
#include <Rendering_Structs_2D.h>
#include "cDynamicBufferT.h"
#include "cTexture.h"


// ** remember to disable depth when rendering


const D3D11_INPUT_ELEMENT_DESC SPRITE_EFFECT_2D_ELEMENT_DESC[2] = 
{
	{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

class c2DSpriteEffect : public iEffect
{
	typedef iEffect Base;
	
	struct sVertex
	{
		// defaults to full-screen
		sVertex() : Position(), HalfWidths(0.5f,0.5f) {}
		cVector2 Position;
		cVector2 HalfWidths;
	};

	typedef cDynamicBufferT<sVertex> cSpriteBuffer;

public:
	
	c2DSpriteEffect();

	virtual ~c2DSpriteEffect();

	virtual void Release();

	virtual bool Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );

	// default rectangle is fullscreen
	virtual void ManualRender( ID3D11DeviceContext* pContext, ID3D11ShaderResourceView* srv, const sRect2D& rect2D = sRect2D(0.f, 0.f, 1.f, 1.f) );


protected:
	cSpriteBuffer mBuffer;
	ID3D11InputLayout*  mLayout;
	ID3DX11EffectShaderResourceVariable* mVarTex;
	ID3DX11EffectTechnique* mTechSprite;
};

#endif  //  __JL_c2DSpriteEffect_HG_20130403__