#pragma once


#include "cPerspectiveTerrainEffect.h"
#include "cModelLoader.h"
#include "cStaticMeshLoader.h"



cPerspectiveTerrainEffect::cPerspectiveTerrainEffect()
{
	mEffectFileName = L"../Resources/Effects/PerspectiveTerrain.fx";
	mMapWidth = 100024.f;
	mMapDepth = 100024.f;
	mHeightMapScale = 400.f;
}

cPerspectiveTerrainEffect::~cPerspectiveTerrainEffect()
{
	this->Release();
}

void cPerspectiveTerrainEffect::Release()
{
	Base::Release();
	mMeshBuffer.Release();
}

bool cPerspectiveTerrainEffect::Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::wstring plyMeshFilePath, std::wstring mapTextureFilePath, std::wstring coarseColourMapFilePath )
{
	if ( !iEffect::LoadFXFile( mEffectFileName, pDevice ) )
	{
		return false;
	}

	if ( !mHeightQuery.Init( pDevice, pContext ) )
	{
		return false;
	}

	// get the technique and the variables
	mTech = mEffect->GetTechniqueByIndex(0);
	mVarCoarseHeightMap = mEffect->GetVariableByName("CoarseHeightMap")->AsShaderResource();
	mVarCoarseColourMap = mEffect->GetVariableByName("CoarseColourMap")->AsShaderResource();
	mVarCoarseHeightMapScale = mEffect->GetVariableByName("CoarseHeightMapScale")->AsScalar();
	mVarMapWidth = mEffect->GetVariableByName("MapWidth")->AsScalar();
	mVarMapDepth = mEffect->GetVariableByName("MapDepth")->AsScalar();
	mVarWorldPositionXZ = mEffect->GetVariableByName("WorldPositionXZ");
	mVarLocalX = mEffect->GetVariableByName("LocalX");
	mVarLocalZ = mEffect->GetVariableByName("LocalZ");
	mVarProjection = mEffect->GetVariableByName("Projection")->AsMatrix();


	if( !mVarMapDepth->IsValid() )
	{ 
		bool shit = true;
	}
	if ( !mVarMapWidth->IsValid() )
	{
		bool shit = true;
	}
	if ( !mVarWorldPositionXZ->IsValid() )
	{
		bool shit = true;
	}
	if (!mVarLocalX->IsValid() )
	{
		bool shit = true;
	}
	if ( !mVarLocalZ->IsValid() )
	{
		bool shit = true;
	}

	// create the input layout
	D3D11_INPUT_ELEMENT_DESC inputElementDesc = 
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	
		D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HRESULT result = pDevice->CreateInputLayout( &inputElementDesc, 1, 
		passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mLayout );
	if ( FAILED( result ) )
	{
		Base::Release();
		return false;
	}

	// load the map
	if ( !mMapTex.LoadFromFile( mapTextureFilePath, pDevice ) )
	{
		return false;
	}
	if ( !mColourMap.LoadFromFile( coarseColourMapFilePath, pDevice ) )
	{
		return false;
	}

	// load the mesh
	cModelLoader modelLoader;
	if ( !modelLoader.Load( plyMeshFilePath ) )
	{
		return false;
	}

	// check that the scene is loaded
	const aiScene* scene = modelLoader.GetScene();
	if ( !scene )
	{
		return false;
	}

	// check that it has a mesh
	if ( !scene->mMeshes || !scene->mMeshes[0] )
	{
		return false;
	}

	// get the mesh
	aiMesh* mesh = scene->mMeshes[0];

	// make sure it has faces and positions
	if ( !mesh->HasPositions() || !mesh->HasFaces() )
	{
		return false;
	}

	// info...
	unsigned int numVerts = mesh->mNumVertices;
	unsigned int numFaces = mesh->mNumFaces;
	unsigned int numInds = numFaces * 3;

	// clear the mesh buffer and initialize it for this mesh
	mMeshBuffer.Release();
	if ( !mMeshBuffer.Init( pDevice, numVerts, numInds ) )
	{
		return false;
	}

	// map in the indices
	unsigned int* inds = mMeshBuffer.BeginIndexMapping( pContext );
	if ( !inds )
	{
		mMeshBuffer.Release();
		return false;
	}
	unsigned int idxIndex = 0;
	for ( unsigned int idxFace = 0; idxFace < numFaces; idxFace++ )
	{
		aiFace* face = &mesh->mFaces[idxFace];
		// we're a triangles only club
		if ( face->mNumIndices != 3 )
		{
			mMeshBuffer.Release();
			return false;
		}

		for ( unsigned int i = 0; i < 3; i++ )
		{
			inds[idxIndex++] = face->mIndices[i];
		}
	}
	mMeshBuffer.FinishIndexMapping( pContext );


	// map in the positions
	cVector2* verts = mMeshBuffer.BeginVertexMapping( pContext );
	if ( !verts )
	{
		mMeshBuffer.Release();
		return false;
	}
	for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
	{
		aiVector3D* a = &mesh->mVertices[idxVert];
		verts[idxVert]= cVector2( a->x, a->z );
	}
	mMeshBuffer.FinishVertexMapping( pContext );

	return true;
}

void cPerspectiveTerrainEffect::Render( ID3D11DeviceContext* pContext, const cVector3& Position, const cQuaternion& Rotation, const cMatrix4x4& proj )
{
	pContext->IASetInputLayout( mLayout );
	mMeshBuffer.SetContext( pContext );

	cVector3 direction = Rotation.GetLocalZAxis();
	cVector3 localZ = direction.ProjectedOn( cVector3(1.f,0.f,0.f), cVector3(0.f,0.f,1.f) );
	direction = Rotation.GetLocalXAxis();
	cVector3 localX = direction.ProjectedOn( cVector3(1.f,0.f,0.f), cVector3(0.f,0.f,1.f) );
	
	cVector2 localX2d( localX.x, localX.z );
	cVector2 localZ2d( localZ.x, localZ.z );
	//cVector2 localX2d( 1.f, 0.f );
	//cVector2 localZ2d( 0.f, 1.f );
	localX2d.Normalize();
	localZ2d.Normalize();
	cVector2 xzPos( Position.x, Position.z );

	mVarProjection->SetMatrix( proj.m );
	
	mVarCoarseHeightMapScale->SetFloat( mHeightMapScale );
	mVarMapWidth->SetFloat( mMapWidth );
	mVarMapDepth->SetFloat( mMapDepth );
	mVarWorldPositionXZ->SetRawValue( &xzPos.x, 0, sizeof(cVector2) );
	mVarLocalX->SetRawValue( &localX2d.x, 0, sizeof(cVector2) );
	mVarLocalZ->SetRawValue( &localZ2d.x, 0, sizeof(cVector2) );
	mVarCoarseHeightMap->SetResource( *mMapTex.GetSRV() );
	mVarCoarseColourMap->SetResource( *mColourMap.GetSRV() );

	mTech->GetPassByIndex(0)->Apply( 0, pContext );
	
	mMeshBuffer.DrawAll( pContext );
}

float cPerspectiveTerrainEffect::GetHeightAt( ID3D11DeviceContext* pContext, float x, float z )
{
	std::vector<cVector2> position( 1, cVector2( x, z ) );
	std::vector<float> height;
	mHeightQuery.DoTheThing( pContext, mMapWidth, mMapDepth, mHeightMapScale, mMapTex.mSRV, position, height );
	return height[0];
}

void cPerspectiveTerrainEffect::GetHeights( ID3D11DeviceContext* pContext, std::vector<cVector2>& positions, std::vector<float>& outputHeights )
{
	mHeightQuery.DoTheThing( pContext, mMapWidth, mMapDepth, mHeightMapScale, mMapTex.mSRV, positions, outputHeights );
}

