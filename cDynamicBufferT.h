#ifndef __JL_cDynamicBufferT_HG_20130317__
#define __JL_cDynamicBufferT_HG_20130317__

#include <D3DX11.h>
#include "eBufferType.h"

template<typename T>
class cDynamicBufferT
{
public:
	cDynamicBufferT( eBufferType bufferType )
		: mDXBufferType(bufferType)
		, mbHasInitialized(false)
		, mNumElements(0)
		, mDXBuffer(0)
	{

	}

	virtual ~cDynamicBufferT()
	{
		this->Release();
	}

	inline bool HasInitialized() const
	{
		return mbHasInitialized;
	}

	inline UINT GetNumElements() const
	{
		return mNumElements;
	}

	inline eBufferType GetBufferType() const
	{
		return mDXBufferType;
	}

	inline ID3D11Buffer* const* GetDXBufferConstPtr() 
	{
		return &mDXBuffer;
	}

	inline ID3D11Buffer* GetDXBuffer() 
	{
		return mDXBuffer;
	}

	/// Initializes this buffer
	/// numElements is only used for index and vertex buffers
	/// for constant buffers, mNumElements will always be 1
	virtual bool Init( ID3D11Device* pDevice, UINT numElements = 1 )
	{
		if ( ( numElements < 1 ) || mbHasInitialized )
		{
			return false;
		}

		mNumElements = numElements;
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;

		switch ( mDXBufferType )
		{
		case E_BUFFER_TYPE_CB:
			{
				mNumElements = 1;
				bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			}
			break;
		case E_BUFFER_TYPE_INDEX:
			{
				bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			}
			break;
		case E_BUFFER_TYPE_VERTEX:
			{
				bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			}
			break;
		default:
			{	// unrecognized type				
				mbHasInitialized = false;
				return false;
			}
			break;
		}

		bd.ByteWidth = sizeof( T )*mNumElements;

		HRESULT result = pDevice->CreateBuffer( &bd, NULL, &mDXBuffer );

		if( FAILED( result ) )
		{
			mbHasInitialized = false;
		}
		else
		{
			mbHasInitialized = true;
		}


		return mbHasInitialized;
	}

	virtual void Release()
	{
		if ( mDXBuffer )
		{
			mDXBuffer->Release();
			mDXBuffer = 0;
		}
		mNumElements = 0;
		mbHasInitialized = false;
	}

	virtual T* BeginMapping( ID3D11DeviceContext* pContext )
	{
		HRESULT result =  pContext->Map( mDXBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mMap );
		if ( FAILED( result ) )
		{
			return 0;
		}
		return reinterpret_cast<T*>( mMap.pData );
	}

	virtual void FinishMapping( ID3D11DeviceContext* pContext )
	{
		pContext->Unmap( mDXBuffer, 0 );
	}


	/// the dx11 buffer
	ID3D11Buffer* mDXBuffer;

private:
	/// the number of elements in this buffer
	/// for constant buffers, this will always be 1
	UINT mNumElements;
	/// has this buffer been initialized
	bool mbHasInitialized;
	/// this buffer's type
	eBufferType mDXBufferType;
	/// the map
	D3D11_MAPPED_SUBRESOURCE mMap;
};

#endif  //  __JL_cDynamicBufferT_HG_20130317__