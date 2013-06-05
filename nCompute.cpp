#include "nCompute.h"

namespace nCompute
{
	HRESULT CreateStructuredBuffer( ID3D11Device* pDevice, UINT StructureByteSize, UINT StructureCount, VOID* pInitData, ID3D11Buffer** ppBuffer )
	{
		*ppBuffer = NULL;
		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.Usage = D3D11_USAGE_DEFAULT;
		//desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = StructureByteSize * StructureCount;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = StructureByteSize;

		if ( pInitData )
		{
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = pInitData;
			return pDevice->CreateBuffer( &desc, &InitData, ppBuffer );
		}
		else
		{
			return pDevice->CreateBuffer( &desc, NULL, ppBuffer );
		}
	}

	HRESULT CreateBufferSRV( ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut )
	{
		D3D11_BUFFER_DESC descBuf;
		ZeroMemory( &descBuf, sizeof(descBuf) );
		pBuffer->GetDesc( &descBuf );
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		desc.BufferEx.FirstElement = 0;

		if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
		{
			// This is a Raw Buffer
			desc.Format = DXGI_FORMAT_R32_TYPELESS;
			desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
			desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
		}
		else
			if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
			{

				// This is a Structured Buffer
				desc.Format = DXGI_FORMAT_UNKNOWN;
				desc.BufferEx.NumElements = 
					descBuf.ByteWidth / descBuf.StructureByteStride;
			}
			else
			{
				return E_INVALIDARG;
			}
			return pDevice->CreateShaderResourceView( pBuffer, &desc, ppSRVOut );
	}

	HRESULT CreateBufferUAV( ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut )
	{
		D3D11_BUFFER_DESC descBuf;
		ZeroMemory( &descBuf, sizeof(descBuf) );
		pBuffer->GetDesc( &descBuf );

		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;

		if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
		{
			// This is a Raw Buffer
			desc.Format = DXGI_FORMAT_R32_TYPELESS;
			// Format must be DXGI_FORMAT_R32_TYPELESS,
			// when creating Raw Unordered Access View

			desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
			desc.Buffer.NumElements = descBuf.ByteWidth / 4; 
		}
		else
			if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
			{
				// This is a Structured Buffer
				desc.Format = DXGI_FORMAT_UNKNOWN;
				// Format must be must be DXGI_FORMAT_UNKNOWN,
				// when creating a View of a Structured Buffer

				desc.Buffer.NumElements = 
					descBuf.ByteWidth / descBuf.StructureByteStride; 
			}
			else
			{
				return E_INVALIDARG;
			}
			return pDevice->CreateUnorderedAccessView( pBuffer, &desc, ppUAVOut );
	}

	HRESULT CreateOutputBuffer( ID3D11DeviceContext* pContext, ID3D11Buffer* pBuffer, ID3D11Buffer** ppOutputBuffer )
	{
		*ppOutputBuffer = NULL;

		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		pBuffer->GetDesc( &desc );
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.MiscFlags = 0;

		ID3D11Device* pDevice = 0;
		pContext->GetDevice( &pDevice );
		return pDevice->CreateBuffer( &desc, NULL, ppOutputBuffer );
	}

	cComputeShader::cComputeShader() : pShader(0)
	{

	}

	cComputeShader::~cComputeShader()
	{
		this->Release();
	}

	void cComputeShader::Release()
	{
		DXRelease( pShader );
	}

	bool cComputeShader::Create( ID3D11Device* pDevice, std::wstring fileName )
	{
		HRESULT result = S_OK;

		ID3DBlob* pBlob = 0;
		if ( !CompileShaderFromFile( fileName, L"CS", L"cs_5_0", &pBlob ) )
		{
			return false;
		}

		result = pDevice->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), 0, &pShader );
		if ( FAILED( result ) )
		{
			DXRelease( pBlob );
			return false;
		}

		return true;
	}

};