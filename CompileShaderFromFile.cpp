#include "CompileShaderFromFile.h"
#include <D3DX11core.h>
#include "nEncoding.h"

bool CompileShaderFromFile( std::wstring shaderFileName, std::wstring entryPoint,
	std::wstring shaderModel, ID3DBlob** ppBlobOut )
{
	HRESULT hr = S_OK;

	bool bInDebugMode = false;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	bInDebugMode = true;
	dwShaderFlags |= D3D10_SHADER_DEBUG;
	dwShaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = NULL;
	std::string ASCIIentryPoint = nEncoding::UNICODEtoASCII( entryPoint);
	std::string ASCIIshaderModel = nEncoding::UNICODEtoASCII( shaderModel);
	hr = D3DX11CompileFromFile( shaderFileName.c_str(), NULL, NULL, 
		ASCIIentryPoint.c_str(), 
		ASCIIshaderModel.c_str(), 
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
	if( FAILED(hr) )
	{
		if( pErrorBlob != NULL )
		{
			std::string ASCIIerror( (char*)pErrorBlob->GetBufferPointer() );
			std::wstring error = nEncoding::ASCIItoUNICODE( ASCIIerror );
			if ( bInDebugMode )
			{
				MessageBox( NULL, error.c_str(), L"Error Compiling Shader!", MB_OK );
			}
			return false;
		}
		if( pErrorBlob ) pErrorBlob->Release();
		return false;
	}
	// Release the blob if was OK
	if( pErrorBlob ) pErrorBlob->Release();

	return true;
}
