#ifndef __JL_cDrawableBuffersT_HG_20130317__
#define __JL_cDrawableBuffersT_HG_20130317__

#include "cRenderBuffersT.h"
#include "cTexture.h"
#include <vector>

template<typename VerType>
class cDrawableBuffersT
{
	struct sModelBufferIndex
	{
		sModelBufferIndex() : BaseVertexLocation(0), Count(0) {}

		void Tick() { Count++; }
		void Tick( UINT ticks ) { Count += ticks; }

		int BaseVertexLocation;
		UINT Count;
	};

public:

	cDrawableBuffersT()
		: mbHasInitialized(false)
		, mRenderBuffers()
	{
		
	}

	virtual ~cDrawableBuffersT()
	{
		this->Release();
	}
	inline bool HasInitialized() const { return mbHasInitialized; }
	inline UINT GetNumVertices() const { return mRenderBuffers.GetNumVertices(); }
	inline UINT GetNumIndices() const { return mRenderBuffers.GetNumIndices(); }

	/// releases everything (called on destruction)
	/// ready to call Init again
	virtual void Release()
	{
		mRenderBuffers.Release();
		mModels.clear();
		mVertices.clear();
		mbHasInitialized = false;
	}


	/// initializes this thing
	/// fails if this has already been initialized (call Release to "Uninitialize" it)
	/// fails if the render buffers fail to initialize
	virtual bool Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, unsigned int numVertices, unsigned int numIndices )
	{
		if ( mbHasInitialized )
		{
			return false;
		}

		if ( !mRenderBuffers.Init( pDevice, numVertices, numIndices ) )
		{
			return false;
		}

		// map the indices
		UINT* inds = mRenderBuffers.BeginIndexMapping( pContext );
		for ( UINT idxIndex = 0; idxIndex < mRenderBuffers.GetNumIndices(); idxIndex++ )
		{
			inds[idxIndex] = idxIndex;
		}
		mRenderBuffers.FinishIndexMapping( pContext );

		mbHasInitialized = true;
		return true;
	}

	/// starts a new model
	/// returns the index of it (for rendering)
	virtual UINT BeginNewModel()
	{
		mModels.push_back( sModelBufferIndex() );
		mModels.back().BaseVertexLocation = mVertices.size();

		return ( mModels.size() - 1 );
	}

	/// adds a vertex to the current model
	virtual void AddVertex( const VerType& vert )
	{
		mModels.back().Tick();
		mVertices.push_back( vert );
	}

	virtual void AddTriangle( const VerType& vertA, const VerType& vertB, const VerType& vertC )
	{
		mModels.back().Tick(3);
		mVertices.push_back( vertA );
		mVertices.push_back( vertB );
		mVertices.push_back( vertC );
	}

	virtual void AddQuad( const VerType& vertA, const VerType& vertB, const VerType& vertC, const VerType& vertD )
	{
		mModels.back().Tick(6);
		mVertices.push_back( vertA );
		mVertices.push_back( vertB );
		mVertices.push_back( vertC );
		mVertices.push_back( vertA );
		mVertices.push_back( vertC );
		mVertices.push_back( vertD );
	}

	/// clears all models
	virtual void ClearModels()
	{
		mModels.clear();
		mVertices.clear();
	}

	/// maps all the vertices to the buffers
	/// and clears the current vertex vector
	/// call this after you're done adding vertices so you can render the models
	/// returns false if this hasn't been initialized
	/// returns false if too many vertices have been added
	virtual bool Commit( ID3D11DeviceContext* pContext )
	{
		if ( !mbHasInitialized )
		{
			return false;
		}

		UINT numVertices = mVertices.size();
		if ( numVertices > mRenderBuffers.GetNumVertices() )
		{
			return false;
		}

		VerType* verts = mRenderBuffers.BeginVertexMapping( pContext );
		for ( UINT idxVertex = 0; idxVertex < numVertices; idxVertex++ )
		{
			verts[idxVertex] = mVertices[idxVertex];
		}
		mRenderBuffers.FinishVertexMapping( pContext );

		return true;
	}

	/// sets the context
	virtual void SetContext( ID3D11DeviceContext* pContext )
	{
		mRenderBuffers.SetContext( pContext );
	}

	/// renders a given model
	virtual bool Render( ID3D11DeviceContext* pContext, UINT modelIndex )
	{
		if ( modelIndex >= mModels.size() )
		{
			return false;
		}

		mRenderBuffers.DrawIndexed( pContext, mModels[modelIndex].Count, 0, mModels[modelIndex].BaseVertexLocation );

		return true;
	}

	cRenderBuffersT<VerType> mRenderBuffers;
protected:
	bool mbHasInitialized;
	
	std::vector<VerType> mVertices;
	std::vector<sModelBufferIndex> mModels;
};


#endif  //  __JL_cDrawableBuffersT_HG_20130317__