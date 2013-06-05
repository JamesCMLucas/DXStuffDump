#ifndef __JL_Terrain_Components_HG_20130405__
#define __JL_Terrain_Components_HG_20130405__

#include <D3DX11.h>
#include <vector>
#include <cVector2.h>
#include "cDynamicBufferT.h"

class cTileRing
{
	struct sAdjacency
	{
		// These are the size of the neighbours along +/- x or y axes.  For interior tiles
		// this is 1.  For edge tiles it is 0.5 or 2.0.
		float NeighbourMinusX;
		float NeighbourMinusY;
		float NeighbourPlusX;
		float NeighbourPlusY;
	};

	struct sInstanceData
	{
		cVector2 Position;
		sAdjacency Adjacency;
	};

	typedef cDynamicBufferT<sInstanceData> cBuffer;

public:
	cTileRing()
		: mBuffer( E_BUFFER_TYPE_VERTEX );
		, mbHasInitialized(false)
		, mInnerWidth(0)
		, mOuterWidth(0)
	{

	}

	virtual ~cTileRing()
	{

	}

	bool Init( ID3D11Device* pDevice, std::vector<int> vecNumTiles )
	{
		if ( mbHasInitialized )
		{
			return false;
		}

		mbHasInitialized = true;
		return true;
	}

	int GetOuterWidth() const { return mOuterWidth; }
	int GetInnerWidth() const { return mInnerWidth; }
	float GetTileSize() const { return mTileSize; }
	bool HasInitialized() const { return mbHasInitialized; }
protected:
	bool CreateVertexBuffer( ID3D11Device* pDevice )
	{

	}

	cBuffer mBuffer;
	bool mbHasInitialized;
	int mOuterWidth;
	int mInnerWidth;
	float mTileSize;

private:
};

#endif  //  __JL_Terrain_Components_HG_20130405__