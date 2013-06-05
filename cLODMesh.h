#ifndef __JL_cLODMesh_HG_20130410__
#define __JL_cLODMesh_HG_20130410__

#include "cRenderBuffersT.h"
#include "cModelLoader.h"
#include "sVert_PNTx.h"
#include <cMatrix4x4.h>
#include <algorithm>
#include <string>
#include <vector>


class cLODMesh
{
	typedef cRenderBuffersT<sVert_PNTx> Buffers;
	struct sBufferContainer
	{
		sBufferContainer() : Buffers(0), MaxDistance(0.f), AvgEdgeSize(0.f) {}
		sBufferContainer( Buffers* bufs, float maxDist )
			: Buffers(bufs), MaxDistance(maxDist), AvgEdgeSize(0.f) {}
		sBufferContainer( Buffers* bufs, float maxDist, float avgEdge )
			: Buffers(bufs), MaxDistance(maxDist), AvgEdgeSize(avgEdge) {}
		sBufferContainer( const sBufferContainer& other )
			: Buffers(other.Buffers), MaxDistance(other.MaxDistance), AvgEdgeSize(other.AvgEdgeSize) {}
		sBufferContainer& operator = ( const sBufferContainer& other )
		{
			if ( &other != this )
			{
				Buffers = other.Buffers;
				MaxDistance = other.MaxDistance;
				AvgEdgeSize = other.AvgEdgeSize;
			}
			return *this;
		}

		Buffers* Buffers;
		float MaxDistance;
		float AvgEdgeSize;
	};
public:
	cLODMesh();
	virtual ~cLODMesh();

	bool LoadModels( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const cMatrix4x4& projMat, std::vector<std::wstring>& fileNames )
	{
		cModelLoader loader;
		std::vector<std::wstring>::iterator itFile = fileNames.begin();
		while ( itFile != fileNames.end() )
		{
			if ( !loader.Load( *itFile ) )
			{
				return false;
			}
			const aiScene* scene = loader.GetScene();
			if ( !scene->HasMeshes() )
			{
				return false;
			}
			aiMesh* mesh = scene->mMeshes[0];
			if ( !mesh->HasPositions() || !mesh->HasFaces() )
			{
				return false;
			}
			if ( !LoadAMeshYo( pDevice, pContext, mesh ) )
			{
				return false;
			}
			itFile++;
		}





		return true;
	}

protected:

	bool LoadAMeshYo( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, aiMesh* mesh )
	{
		unsigned int numVerts = mesh->mNumVertices;
		unsigned int numFaces = mesh->mNumFaces;
		unsigned int numInds = numFaces*3;
		float avgEdgeSize = 0.f;

		Buffers* buffers = new Buffers();
		if ( !buffers->Init( pDevice, numVerts, numInds ) )
		{
			return false;
		}

		// figure out the test faces
		unsigned int numTestFaces = numFaces / 100;
		if ( numTestFaces == 0 )
		{
			numTestFaces = 1;
		}
		std::vector<unsigned int> testFaces;
		for ( unsigned int idxTestFace = 0; idxTestFace < numTestFaces; idxTestFace++ )
		{
			unsigned int randIndex = 0;
			bool checksout = false;
			while ( !checksout )
			{
				randIndex = static_cast<unsigned int>( ( rand() / RAND_MAX )*numFaces );
				if ( std::find( testFaces.begin(), testFaces.end(), randIndex ) != testFaces.end()  )
				{
					testFaces.push_back( randIndex );
					checksout = true;
				}
			}
		}

		for ( unsigned int idxFace = 0; idxFace < numTestFaces; idxFace++ )
		{
			aiFace* face = &mesh->mFaces[testFaces[idxFace]];
			// take edge 0 (v0-v1)
			aiVector3D* v0 = &mesh->mVertices[face->mIndices[0]];
			aiVector3D* v1 = &mesh->mVertices[face->mIndices[1]];
			aiVector3D vR = (*v0) - (*v1);
			avgEdgeSize += vR.Length();
		}
		avgEdgeSize /= static_cast<float>( numTestFaces );



		// map the indices
		unsigned int* inds = buffers->BeginIndexMapping( pContext );
		unsigned int idxIndex = 0;
		for ( unsigned int idxFace = 0; idxFace < numFaces; idxFace++ )
		{
			aiFace* f = &mesh->mFaces[idxFace];
			if ( f->mNumIndices != 3 )
			{
				// not a triangle
				buffers->FinishIndexMapping( pContext );
				delete buffers;
				return false;
			}
			for ( unsigned int i = 0; i < 3; i++ )
			{
				inds[idxIndex++] = f->mIndices[i];
			}
		}
		buffers->FinishIndexMapping( pContext );

		// map the vertices
		sVert_PNTx* verts = buffers->BeginVertexMapping( pContext );

		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			aiVector3D* a = &mesh->mVertices[idxVert];
			verts[idxVert].Position = cVector3( a->x, a->y, a->z );
			verts[idxVert].PositionPad = 1.f;
			verts[idxVert].NormalPad = 1.f;
		}
		if ( mesh->HasNormals() )
		{
			for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
			{
				aiVector3D* n = &mesh->mNormals[idxVert];
				verts[idxVert].Normal = cVector3( n->x, n->y, n->z );
			}
		}
		if ( mesh->HasTextureCoords(0) )
		{
			for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
			{
				aiVector3D* t = &mesh->mTextureCoords[0][idxVert];
				verts[idxVert].TexCoord0 = cVector2( t->x, t->y );
			}
		}
		
		buffers->FinishVertexMapping( pContext );


		mBuffers.push_back( sBufferContainer( buffers, 1000.f, avgEdgeSize ) );
	}

	bool DoTheCalculationThing( const cMatrix4x4& proj )
	{
		// TODO: CHANGE THE CONDITION TO MATCH PIXEL SIZES
		// right now just ordering them by average edge size ( small to big )
		//std::vector<float> pixelSizes( mBuffers.size() );
		for ( unsigned int i = 0; i < mBuffers.size() - 1; i++ )
		{
			for ( unsigned int j = i + 1; j < mBuffers.size(); j++ )
			{
				if ( mBuffers[j].AvgEdgeSize < mBuffers[i].AvgEdgeSize )
				{
					std::swap( mBuffers[i], mBuffers[j] );
				}
			}
		}
	}

	std::vector<sBufferContainer> mBuffers;

};

#endif  //  __JL_cLODMesh_HG_20130410__