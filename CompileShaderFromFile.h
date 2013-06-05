#include <string>
#ifndef __JL_CompileShaderFromFile_HG_20130317__
#define __JL_CompileShaderFromFile_HG_20130317__

#include <D3D11.h>
#include <D3Dcompiler.h>
#include <string>


bool CompileShaderFromFile( std::wstring shaderFileName, std::wstring entryPoint,
	std::wstring shaderModel, ID3DBlob** ppBlobOut );

#endif  //  __JL_CompileShaderFromFile_HG_20130317__