#pragma once

#include "iEffect.h"
#include "nEncoding.h"


iEffect::iEffect()
	: mbInitialized(false)
	, NecessaryPerFrame(EFFECT_VAR_USAGE_NECESSARY_PER_FRAME)
	, OptionalPerFrame(EFFECT_VAR_USAGE_OPTIONAL_PER_FRAME)
	, NecessaryPerObject(EFFECT_VAR_USAGE_NECESSARY_PER_OBJECT)
	, OptionalPerObject(EFFECT_VAR_USAGE_OPTIONAL_PER_OBJECT)
	, mbResetOptionalPerFrameAfterUse(false)
	, mbResetOptionalPerObjectAfterUse(true)
{

}

iEffect::~iEffect()
{
	this->Release();
}

void iEffect::Release()
{
	if ( mEffect )
	{
		mEffect->Release();
		mEffect = 0;
	}
	mbInitialized = false;
}

bool iEffect::LoadFXFile( std::wstring fileName, ID3D11Device* pDevice )
{
	if ( mbInitialized )
	{
		return false;
	}

	mEffectFileName = fileName;

	HRESULT result = S_OK;

	DWORD shaderFlags = 0;
	bool bDebugMode = false;
#if defined(DEBUG) || defined(_DEBUG)
	bDebugMode = true;
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* pShaderBlob = 0;
	ID3D10Blob* pErrorBlob = 0;

	result = D3DX11CompileFromFile( 
		mEffectFileName.c_str(),  /* file name */
		NULL,		/* no defines */
		NULL,		/* no includes */
		NULL,		/* using effects framework so no entry point specified */
		"fx_5_0",	/* using Direct3D 11 effects.. shader version 5.0 */
		shaderFlags,/* specify flags */
		0,			/* advanced effect compiling options */
		0,			/* thread pump... if compiling asynchronously */
		&pShaderBlob,		/* output: the compiled shader structure */
		&pErrorBlob,		/* output: errors */
		NULL				/* output: errors... if compiling asynchronously */
		);

	// failed!...?
	if ( FAILED( result ) )
	{
		// check for recorded errors
		if ( pErrorBlob != 0 )
		{
			if ( bDebugMode )
			{
				std::string error = reinterpret_cast<char*>( pErrorBlob->GetBufferPointer() );
				std::wstring werror = nEncoding::ASCIItoUNICODE( error );
				MessageBox( NULL, werror.c_str(), L"iEffect error: couldn't load fx file", MB_OK );
			}
			pErrorBlob->Release();
			return false;
		}
		if ( bDebugMode )
		{
			MessageBox( NULL, L"Unknown error...", L"iEffect error: couldn't load fx file", MB_OK );
		}
		return false;
	}

	// check for warnings
	if ( pErrorBlob )
	{
		if ( bDebugMode )
		{
			std::string warning = reinterpret_cast<char*>( pErrorBlob->GetBufferPointer() );
			std::wstring wWarning = nEncoding::ASCIItoUNICODE( warning );
			MessageBox( NULL, wWarning.c_str(), L"iEffect : got some warnings loading fx file", MB_OK );
		}
		pErrorBlob->Release();
	}

	// create the effect
	result = D3DX11CreateEffectFromMemory( 
		pShaderBlob->GetBufferPointer(),	/* the compiled shader data */
		pShaderBlob->GetBufferSize(),		/* size of the compiled shader */
		0,									/* any flags */
		pDevice,							/* the device */
		&mEffect								/* output: the effect */
		);

	if ( FAILED( result ) )
	{
		if ( bDebugMode )
		{
			MessageBox( NULL, L"Couldn't create the effect!", L"iEffect error", MB_OK );
		}
		return false;
	}

	// done with mr shader blob
	pShaderBlob->Release();
	

	// horray! we're initialized!
	mbInitialized = true;
	return true;
}