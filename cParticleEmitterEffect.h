#ifndef __JL_cParticleEmitterEffect_HG_20130403__
#define __JL_cParticleEmitterEffect_HG_20130403__

#include "iEffect.h"

#include <cTimer.h>

const D3D11_INPUT_ELEMENT_DESC PARTICLE_EMITTER_ELEMENT_DESC[5] = 
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

class cParticleEmitterEffect : public iEffect
{
	struct sParticle
	{
		sParticle()
		{
			Position.Zeroes();
			Velocity.Zeroes();
			HalfWidths.Ones();
			Age = 0.f;
			Type = 0;
		}
		cVector3		Position;
		cVector3		Velocity;
		cVector2		HalfWidths;
		float			Age;
		unsigned int	Type;
	};
public:
	cParticleEmitterEffect();
	virtual ~cParticleEmitterEffect();

	virtual void SetEmitterPosition( const cVector3& emitterPos );
	virtual void SetEmitterDirection( const cVector3& emitterDir );
	virtual void SetParticleAcceleration( const cVector3& accel );
	virtual void SetMaxParticleAge( float maxAge );
	virtual bool Init( unsigned int maxParticles, ID3D11Device* pDevice, ID3D11DeviceContext* pContext );

	virtual void Reset();
	
	virtual void ManualRender( ID3D11DeviceContext* pContext, const cMatrix4x4& viewProj, const cVector3& eyePos );

	virtual void FinishScene( ID3D11DeviceContext* pContext );
protected:
	cVector3 mParticleAcceleration;
	float mMaxParticleAge;
	cTimer mTimer;
	unsigned int mMaxNumParticles;

	bool mbFirstRun;
	float mAge;

	cVector3 mEmitterPosition;
	cVector3 mEmitterDirection;

	ID3D11Buffer* mInitVB;	
	ID3D11Buffer* mDrawVB;
	ID3D11Buffer* mStreamOutVB;
	ID3D11InputLayout*  mLayout;
	std::wstring mTexFileName;
	ID3D11ShaderResourceView* mTexArraySRV;
	ID3D11ShaderResourceView* mRandomTexSRV;

	ID3DX11EffectTechnique* mTechStreamOut;
	ID3DX11EffectTechnique* mTechDraw;

	ID3DX11EffectVectorVariable* mVarParticleAccelerations;

	ID3DX11EffectMatrixVariable* mVarViewProj;
	ID3DX11EffectScalarVariable* mVarMaxAge;
	ID3DX11EffectScalarVariable* mVarGameTime;
	ID3DX11EffectScalarVariable* mVarTimeStep;
	ID3DX11EffectVectorVariable* mVarEyePosition;
	ID3DX11EffectVectorVariable* mVarEmitterPosition;
	ID3DX11EffectVectorVariable* mVarEmitterDirection;
	ID3DX11EffectShaderResourceVariable* mVarTexArray;
	ID3DX11EffectShaderResourceVariable* mVarRandomTex;
};

#endif  //  __JL_cParticleEmitterEffect_HG_20130403__