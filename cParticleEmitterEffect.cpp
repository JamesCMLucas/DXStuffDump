#include "cParticleEmitterEffect.h"
#include <cRNG.h>


cParticleEmitterEffect::cParticleEmitterEffect()
	: mInitVB(0)
	, mDrawVB(0)
	, mStreamOutVB(0)
	, mTexArraySRV(0)
	, mRandomTexSRV(0)
	, mLayout(0)
{
	mbFirstRun = true;
	mAge = 0.0f;
	mMaxParticleAge = 0.005f;
	mTimer.Reset();
	mEmitterPosition = cVector3( 0.f, 0.f, 0.f );
	mEmitterDirection = cVector3( 0.f, 1.f, 0.f );
	mParticleAcceleration = cVector3( 0.f, 10.0f, 0.f );
	mEffectFileName = L"../Resources/Effects/ParticleEmitter_Fire.fx";
	mTexFileName = L"../Resources/Tex/DDS/flare0.dds";
}

cParticleEmitterEffect::~cParticleEmitterEffect()
{
	this->Release();
}

void cParticleEmitterEffect::SetEmitterPosition( const cVector3& emitterPos )
{
	mEmitterPosition = emitterPos;
}

void cParticleEmitterEffect::SetEmitterDirection( const cVector3& emitterDir )
{
	mEmitterDirection = emitterDir;
}

void cParticleEmitterEffect::SetParticleAcceleration( const cVector3& accel )
{
	mParticleAcceleration = accel;
}

void cParticleEmitterEffect::SetMaxParticleAge( float maxAge )
{
	mMaxParticleAge = maxAge;
}

bool cParticleEmitterEffect::Init( unsigned int maxParticles, ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	if ( mbInitialized )
	{
		return false;
	}
	mMaxNumParticles = maxParticles;

	if ( !iEffect::LoadFXFile( mEffectFileName, pDevice ) )
	{
		return false;
	}

	// create the input layout
	D3DX11_PASS_DESC passDesc;
	mEffect->GetTechniqueByIndex(0)->GetPassByIndex(0)->GetDesc(&passDesc);
	HRESULT result = pDevice->CreateInputLayout( PARTICLE_EMITTER_ELEMENT_DESC, 5, 
		passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mLayout );
	if ( FAILED( result ) )
	{
		iEffect::Release();
		return false;
	}

	// -------------- CREATING THE SRV ARRAY ( JUST THE ONE RIGHT NOW )
	ID3D11Texture2D* pTex;
	D3DX11_IMAGE_LOAD_INFO loadInfo;
	loadInfo.Width  = D3DX11_FROM_FILE;
	loadInfo.Height = D3DX11_FROM_FILE;
	loadInfo.Depth  = D3DX11_FROM_FILE;
	loadInfo.FirstMipLevel = 0;
	loadInfo.MipLevels = D3DX11_FROM_FILE;
	loadInfo.Usage = D3D11_USAGE_STAGING;
	loadInfo.BindFlags = 0;
	loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	loadInfo.MiscFlags = 0;
	loadInfo.Format = DXGI_FORMAT_FROM_FILE;
	loadInfo.Filter = D3DX11_FILTER_NONE;
	loadInfo.MipFilter = D3DX11_FILTER_LINEAR;
	loadInfo.pSrcInfo  = 0;

	result = D3DX11CreateTextureFromFile( pDevice, mTexFileName.c_str(),
		&loadInfo, 0, (ID3D11Resource**)&pTex, 0 );
	if ( FAILED( result ) )
	{
		iEffect::Release();
		return false;
	}
	D3D11_TEXTURE2D_DESC texElementDesc;
	pTex->GetDesc( &texElementDesc );
	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width              = texElementDesc.Width;
	texArrayDesc.Height             = texElementDesc.Height;
	texArrayDesc.MipLevels          = texElementDesc.MipLevels;
	texArrayDesc.ArraySize          = 1; // only one right now!
	texArrayDesc.Format             = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count   = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage              = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags     = 0;
	texArrayDesc.MiscFlags          = 0;

	ID3D11Texture2D* texArray = 0;
	pDevice->CreateTexture2D( &texArrayDesc, 0, &texArray );
	for ( UINT idxMipLevel = 0; idxMipLevel < texElementDesc.MipLevels; idxMipLevel++ )
	{
		D3D11_MAPPED_SUBRESOURCE mappedTex2D;
		result = pContext->Map( pTex, idxMipLevel, D3D11_MAP_READ, 0, &mappedTex2D );
		if ( FAILED(result))
		{
			bool shit = true;
		}
		pContext->UpdateSubresource( texArray,
			D3D11CalcSubresource(idxMipLevel, 0/*idxTexArray*/, texElementDesc.MipLevels),
			0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch );
		pContext->Unmap( pTex, idxMipLevel );
	}

	// create the resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = 1;  // array of one right now!

	mTexArraySRV = 0;
	result = pDevice->CreateShaderResourceView( texArray, &viewDesc, &mTexArraySRV );
	if ( FAILED(result))
	{
		bool shit = true;
	}

	// release the "array"
	pTex->Release();


	// -------------- CREATING A RANDOM 1D TEXTURE
	cRNG rng(static_cast<unsigned long>(rand()));
	cVector4 randoms[1024];
	for ( int i = 0; i < 1024; i++ )
	{
		randoms[i].x = static_cast<float>(rng.Unif( -1.0, 1.0 ));
		randoms[i].y = static_cast<float>(rng.Unif( -1.0, 1.0 ));
		randoms[i].z = static_cast<float>(rng.Unif( -1.0, 1.0 ));
		randoms[i].w = static_cast<float>(rng.Unif( -1.0, 1.0 ));
	}
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = randoms;
	initData.SysMemPitch = 1024*sizeof(cVector4);
	initData.SysMemSlicePitch = 0;

	// create the texture
	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width = 1024;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;

	ID3D11Texture1D* randomTex = 0;
	result = pDevice->CreateTexture1D( &texDesc, &initData, &randomTex );
	if ( FAILED( result ) )
	{
		return false;
	}

	// create the srv
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc1d;
	viewDesc1d.Format = texDesc.Format;
	viewDesc1d.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc1d.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc1d.Texture1D.MostDetailedMip = 0;

	mRandomTexSRV = 0;
	result = pDevice->CreateShaderResourceView( randomTex, &viewDesc1d, &mRandomTexSRV );
	if ( FAILED( result ) )
	{
		return false;
	}

	randomTex->Release();




	//     MAKE THE VERTEX BUFFERS
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(sParticle) * 1;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	sParticle particle = sParticle();

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &particle;

	result = pDevice->CreateBuffer( &vbd, &vinitData, &mInitVB );
	if ( FAILED( result ) )
	{
		return false;
	}

	vbd.ByteWidth = sizeof(sParticle) * mMaxNumParticles;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	result = pDevice->CreateBuffer( &vbd, 0, &mDrawVB );
	if ( FAILED( result ) )
	{
		return false;
	}
	result = pDevice->CreateBuffer( &vbd, 0, &mStreamOutVB );
	if ( FAILED( result ) )
	{
		return false;
	}

	// necessary per frame
	ID3DX11EffectVariable* viewProj = mEffect->GetVariableByName("ViewProjMatrix");
	ID3DX11EffectVariable* eyePos = mEffect->GetVariableByName("EyePosition");
	NecessaryPerFrame.push_back( new cMatrixEffectVariable( viewProj ) );
	NecessaryPerFrame.push_back( new cFloat3EffectVariable( eyePos ) );
	mVarViewProj = viewProj->AsMatrix();
	mVarEyePosition = eyePos->AsVector();
	
	// optional per frame
	ID3DX11EffectVariable* emitterPos = mEffect->GetVariableByName("EmitterPosition");
	OptionalPerFrame.push_back( new cFloat3EffectVariable( emitterPos ) );
	mVarEmitterPosition = emitterPos->AsVector();

	// GET THE VARIABLES
	mVarMaxAge = mEffect->GetVariableByName("MaxAge")->AsScalar();
	mVarParticleAccelerations = mEffect->GetVariableByName("ParticleAcceleration")->AsVector();
	
	mVarGameTime = mEffect->GetVariableByName("TotalTime")->AsScalar();
	mVarTimeStep = mEffect->GetVariableByName("TimeStep")->AsScalar();
	
	mVarEmitterDirection = mEffect->GetVariableByName("EmitterDirection")->AsVector();
	mVarTexArray = mEffect->GetVariableByName("TexArray")->AsShaderResource();
	mVarRandomTex = mEffect->GetVariableByName("RandomTex")->AsShaderResource();

	// GET THE TECHNIQUES
	mTechStreamOut = mEffect->GetTechniqueByIndex(0);
	mTechDraw = mEffect->GetTechniqueByIndex(1);



	mbInitialized = true;
	return true;
}

void cParticleEmitterEffect::Reset()
{
	mbFirstRun = true;
	mAge = 0.f;
	mTimer.Restart();
}

void cParticleEmitterEffect::ManualRender( ID3D11DeviceContext* pContext, const cMatrix4x4& viewProj, const cVector3& eyePos )
{
	float dt = mTimer.Restart();
	mAge += dt;

	mVarParticleAccelerations->SetRawValue( &mParticleAcceleration.x, 0, sizeof(cVector3) );

	
	mVarViewProj->SetMatrix( viewProj.m );

	mVarEyePosition->SetRawValue( &eyePos.x, 0, sizeof(cVector3) );
	mVarTimeStep->SetFloat( dt );
	mVarGameTime->SetFloat( mAge );
	mVarMaxAge->SetFloat( mMaxParticleAge );
	mVarEmitterPosition->SetRawValue( &mEmitterPosition.x, 0, sizeof(cVector3) );
	mVarEmitterDirection->SetRawValue( &mEmitterDirection.x, 0, sizeof(cVector3) );
	mVarTexArray->SetResource( mTexArraySRV );
	mVarRandomTex->SetResource( mRandomTexSRV );

	// set IA stage:
	// set input layout
	pContext->IASetInputLayout( mLayout );
	pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );

	UINT stride = sizeof(sParticle);
	UINT offset = 0;

	// set the vertex buffers
	if ( mbFirstRun )
	{
		pContext->IASetVertexBuffers( 0, 1, &mInitVB, &stride, &offset );
	}
	else
	{
		pContext->IASetVertexBuffers( 0, 1, &mDrawVB, &stride, &offset );
	}


	pContext->SOSetTargets( 1, &mStreamOutVB, &offset );

	D3DX11_TECHNIQUE_DESC tDesc;
	mTechStreamOut->GetDesc( &tDesc );
	for ( unsigned int idxPass = 0; idxPass < tDesc.Passes; idxPass++ )
	{
		mTechStreamOut->GetPassByIndex( idxPass )->Apply( 0, pContext );

		if ( mbFirstRun )
		{
			pContext->Draw( 1, 0 );
			mbFirstRun = false;
		}
		else
		{
			pContext->DrawAuto();
		}
	}

	// bind vertex buffer
	ID3D11Buffer* bufferArray[1] = {0};
	pContext->SOSetTargets( 1, bufferArray, &offset );
	// ping pong
	std::swap( mDrawVB, mStreamOutVB );

	// draw updated particle system
	pContext->IASetVertexBuffers( 0, 1, &mDrawVB, &stride, &offset );

	mTechDraw->GetDesc( &tDesc );
	for ( unsigned int idxPass = 0; idxPass < tDesc.Passes; idxPass++ )
	{
		mTechDraw->GetPassByIndex( idxPass )->Apply( 0, pContext );
		pContext->DrawAuto();
	}

	pContext->GSSetShader( 0, 0, 0 );
}

void cParticleEmitterEffect::FinishScene( ID3D11DeviceContext* pContext )
{
	pContext->VSSetShader( 0, 0, 0 );
	pContext->GSGetShader( 0, 0, 0 );
	pContext->PSSetShader( 0, 0, 0 );
}
