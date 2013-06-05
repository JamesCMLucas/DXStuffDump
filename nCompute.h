#ifndef __JL_nCompute_HG_20130411__
#define __JL_nCompute_HG_20130411__

#include "DXBase.h"
#include "CompileShaderFromFile.h"


namespace nCompute
{
	/*! Creates a structured buffer
	*/
	HRESULT CreateStructuredBuffer( ID3D11Device* pDevice, 
		UINT StructureByteSize, UINT StructureCount, VOID* pInitData, 
		ID3D11Buffer** ppBuffer );

	/*! Creates a shader resource view for a buffer
	*/
	HRESULT CreateBufferSRV( ID3D11Device* pDevice, ID3D11Buffer* pBuffer, 
		ID3D11ShaderResourceView** ppSRVOut );

	/*! Creates an unordered access view for a buffer
	*/
	HRESULT CreateBufferUAV( ID3D11Device* pDevice, ID3D11Buffer* pBuffer, 
		ID3D11UnorderedAccessView** ppUAVOut );

	/*! Creates an output buffer
	*/
	HRESULT CreateOutputBuffer( ID3D11DeviceContext* pContext, ID3D11Buffer* pBuffer, ID3D11Buffer** ppOutputBuffer );

	/*! Templated input buffer
	*/
	template<class T>
	class cInputBufferT
	{
	public:
		cInputBufferT()
			: pDynamicBuffer(0)
			, pSRV(0)
		{
			
		}

		virtual ~cInputBufferT()
		{
			this->Release();
		}

		void Release()
		{
			DXRelease( pDynamicBuffer );
			DXRelease( pSRV );
		}

		bool Init( ID3D11DeviceContext* pContext, unsigned int numElements, void* initData = 0 )
		{
			this->Release();
			HRESULT result = S_OK;

			ID3D11Device* pDevice = 0;
			pContext->GetDevice( &pDevice );

			D3D11_BUFFER_DESC outputDesc;
			outputDesc.Usage = D3D11_USAGE_DYNAMIC;
			outputDesc.ByteWidth = sizeof(T) * numElements;
			outputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			outputDesc.StructureByteStride = sizeof(T);
			outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

			if ( FAILED( pDevice->CreateBuffer( &outputDesc, 0, &pDynamicBuffer ) ) )
			{
				return false;
			}
			
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			srvDesc.BufferEx.FirstElement = 0;
			srvDesc.BufferEx.Flags = 0;
			srvDesc.BufferEx.NumElements = numElements;

			if ( FAILED( pDevice->CreateShaderResourceView( pDynamicBuffer, &srvDesc, &pSRV ) ) )
			{
				return false;
			}

			return true;
		}

		T* BeginInputMapping( ID3D11DeviceContext* pContext )
		{
			HRESULT result =  pContext->Map( pDynamicBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ResourceMap );
			if ( FAILED( result ) )
			{
				return 0;
			}
			return reinterpret_cast<T*>( ResourceMap.pData );
		}

		void FinishInputMapping( ID3D11DeviceContext* pContext )
		{
			pContext->Unmap( pDynamicBuffer, 0 );
			//pContext->CopyResource( pDefaultBuffer, pStagingBuffer );
		}

		D3D11_MAPPED_SUBRESOURCE ResourceMap;
		ID3D11Buffer* pDynamicBuffer;
		ID3D11ShaderResourceView* pSRV;
	};

	/*! Shader
	*/
	class cComputeShader
	{
	public:
		cComputeShader();
		virtual ~cComputeShader();

		virtual void Release();

		bool Create( ID3D11Device* pDevice, std::wstring fileName );

		ID3D11ComputeShader* pShader;
	};


	/*! Templated read/write buffer
	*/
	template<class T>
	class cOutputBufferT
	{
	public:
		cOutputBufferT()
			: pDefaultBuffer(0)
			, pStagingBuffer(0)
			, pUAV(0)
		{
			
		}

		virtual ~cOutputBufferT()
		{
			this->Release();
		}

		void Release()
		{
			DXRelease( pDefaultBuffer );
			DXRelease( pStagingBuffer );
			DXRelease( pUAV );
		}

		bool Init( ID3D11DeviceContext* pContext, unsigned int numElements, void* initData = 0 )
		{
			this->Release();
			HRESULT result = S_OK;

			ID3D11Device* pDevice = 0;
			pContext->GetDevice( &pDevice );

			D3D11_BUFFER_DESC outputDesc;
			outputDesc.Usage = D3D11_USAGE_DEFAULT;
			outputDesc.ByteWidth = sizeof(T) * numElements;
			outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			outputDesc.CPUAccessFlags = 0;
			outputDesc.StructureByteStride = sizeof(float);
			outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

			if ( FAILED( pDevice->CreateBuffer( &outputDesc, 0, &pDefaultBuffer ) ) )
			{
				return false;
			}

			// Create a system memory version of the buffer to read the results back from.
			outputDesc.Usage = D3D11_USAGE_STAGING;
			outputDesc.BindFlags = 0;
			outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			if ( FAILED( pDevice->CreateBuffer( &outputDesc, 0, &pStagingBuffer ) ) )
			{
				return false;
			}

			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.Flags = 0;
			uavDesc.Buffer.NumElements = numElements;

			if ( FAILED( pDevice->CreateUnorderedAccessView( pDefaultBuffer, &uavDesc, &pUAV ) ) )
			{
				return false;
			}


			return true;
		}

		T* BeginOutputMapping( ID3D11DeviceContext* pContext )
		{
			pContext->CopyResource( pStagingBuffer, pDefaultBuffer );
			HRESULT result =  pContext->Map( pStagingBuffer, 0, D3D11_MAP_READ, 0, &ResourceMap );
			if ( FAILED( result ) )
			{
				return 0;
			}
			return reinterpret_cast<T*>( ResourceMap.pData );
		}

		void FinishOutputMapping( ID3D11DeviceContext* pContext )
		{
			pContext->Unmap( pStagingBuffer, 0 );
		}

		D3D11_MAPPED_SUBRESOURCE ResourceMap;
		ID3D11Buffer* pDefaultBuffer;
		ID3D11Buffer* pStagingBuffer;
		ID3D11UnorderedAccessView* pUAV;
	};
}



#endif  //  __JL_nCompute_HG_20130411__