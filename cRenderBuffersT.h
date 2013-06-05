#ifndef __JL_cRenderBuffersT_HG_20130317__
#define __JL_cRenderBuffersT_HG_20130317__

#include "cDynamicBufferT.h"

template<typename VerType>
class cRenderBuffersT
{
public:
	cRenderBuffersT()
		: mbHasInitialized(false)
		, mVertexBuffer(E_BUFFER_TYPE_VERTEX)
		, mIndexBuffer(E_BUFFER_TYPE_INDEX)
		, mStride( sizeof(VerType) )
		, mOffset(0)
	{
		mPrimitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	virtual ~cRenderBuffersT()
	{
		this->Release();
	}

	inline bool HasInitialized() const
	{
		return ( mVertexBuffer.HasInitialized() && mIndexBuffer.HasInitialized() );
	}

	inline UINT GetNumIndices() const 
	{
		return mIndexBuffer.GetNumElements();
	}
	
	inline UINT GetNumVertices() const
	{
		return mVertexBuffer.GetNumElements();
	}

	inline D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const
	{
		return mPrimitiveTopology;
	}

	inline VerType* BeginVertexMapping( ID3D11DeviceContext* pContext )
	{
		return mVertexBuffer.BeginMapping( pContext );
	}

	inline VerType* BeginVertexMapping( ID3D11DeviceContext* pContext ) const
	{
		return mVertexBuffer.BeginMapping( pContext );
	}

	inline void FinishVertexMapping( ID3D11DeviceContext* pContext )
	{
		mVertexBuffer.FinishMapping( pContext );
	}

	inline void FinishVertexMapping( ID3D11DeviceContext* pContext ) const
	{
		mVertexBuffer.FinishMapping( pContext );
	}

	inline UINT* BeginIndexMapping( ID3D11DeviceContext* pContext )
	{
		return mIndexBuffer.BeginMapping( pContext );
	}

	inline UINT* BeginIndexMapping( ID3D11DeviceContext* pContext ) const
	{
		return mIndexBuffer.BeginMapping( pContext );
	}

	inline void FinishIndexMapping( ID3D11DeviceContext* pContext )
	{
		mIndexBuffer.FinishMapping( pContext );
	}

	inline void FinishIndexMapping( ID3D11DeviceContext* pContext ) const
	{
		mIndexBuffer.FinishMapping( pContext );
	}

	virtual void Release()
	{
		mVertexBuffer.Release();
		mIndexBuffer.Release();
		mbHasInitialized = false;
	}

	bool Init( ID3D11Device* pDevice, unsigned int numVertices, unsigned int numIndices )
	{
		if ( mbHasInitialized || ( numVertices < 1 ) || ( numIndices < 1 ) )
		{
			return false;
		}

		if ( !mVertexBuffer.Init( pDevice, numVertices ) )
		{
			return false;
		}
		if ( !mIndexBuffer.Init( pDevice, numIndices ) )
		{
			mVertexBuffer.Release();
			return false;
		}

		return true;
	}

	void SetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY topology )
	{
		mPrimitiveTopology = topology;
	}

	void SetContext( ID3D11DeviceContext* pContext )
	{
		pContext->IASetPrimitiveTopology( mPrimitiveTopology );
		pContext->IASetVertexBuffers( 0, 1, &mVertexBuffer.mDXBuffer,/*mVertexBuffer.GetDXBufferConstPtr(),*/ &mStride, &mOffset );
		pContext->IASetIndexBuffer( mIndexBuffer.mDXBuffer,/*mIndexBuffer.GetDXBuffer(),*/ DXGI_FORMAT_R32_UINT, 0 );
	}

	void DrawIndexed( ID3D11DeviceContext* pContext, unsigned int indexCount, unsigned int startIndexLocation, int baseVertexLocation )
	{
		pContext->DrawIndexed( indexCount, startIndexLocation, baseVertexLocation );
	}

	void DrawAll( ID3D11DeviceContext* pContext )
	{
		pContext->DrawIndexed( mIndexBuffer.GetNumElements(), 0, 0 );
	}
	
protected:
	/// the vertex buffer
	cDynamicBufferT<VerType>	mVertexBuffer;
	/// the index buffer
	cDynamicBufferT<UINT>		mIndexBuffer;
	/// vertex buffer stride = sizeof(VerType)
	UINT						mStride;
	/// vertex buffer offset = 0
	UINT						mOffset;
	/// the primitive topology
	/// default is triangle lists
	D3D_PRIMITIVE_TOPOLOGY		mPrimitiveTopology;

private:
	bool mbHasInitialized;
};

#endif  //  __JL_cRenderBuffersT_HG_20130317__