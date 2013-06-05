#include "cStaticMeshLoader.h"
#include "cModelLoader.h"


bool cStaticMeshLoader::Load_P( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, cModelLoader& loader, cRenderBuffersT<sVert_P>& meshBuffer )
{
	// check that the scene is loaded
	const aiScene* scene = loader.GetScene();
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
	meshBuffer.Release();
	if ( !meshBuffer.Init( pDevice, numVerts, numInds ) )
	{
		return false;
	}



	// map in the indices

	unsigned int* inds = meshBuffer.BeginIndexMapping( pContext );
	if ( !inds )
	{
		meshBuffer.Release();
		return false;
	}
	unsigned int idxIndex = 0;
	for ( unsigned int idxFace = 0; idxFace < numFaces; idxFace++ )
	{
		aiFace* face = &mesh->mFaces[idxFace];
		// we're a triangles only club
		if ( face->mNumIndices != 3 )
		{
			meshBuffer.Release();
			return false;
		}

		for ( unsigned int i = 0; i < 3; i++ )
		{
			inds[idxIndex++] = face->mIndices[i];
		}
	}
	meshBuffer.FinishIndexMapping( pContext );
	
	
	
	// map in the positions

	sVert_P* verts = meshBuffer.BeginVertexMapping( pContext );
	if ( !verts )
	{
		meshBuffer.Release();
		return false;
	}
	for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
	{
		aiVector3D* a = &mesh->mVertices[idxVert];
		verts[idxVert].Position = cVector3( a->x, a->y, a->z );
		verts[idxVert].PositionPad = 1.f;
	}
	meshBuffer.FinishVertexMapping( pContext );
	
	return true;
}

bool cStaticMeshLoader::Load_PN( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, cModelLoader& loader, cRenderBuffersT<sVert_PN>& meshBuffer )
{
	// check that the scene is loaded
	const aiScene* scene = loader.GetScene();
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

	// make sure it has faces and positions and normals
	if ( !mesh->HasPositions() || !mesh->HasFaces() || !mesh->HasNormals() )
	{
		return false;
	}

	// info...
	unsigned int numVerts = mesh->mNumVertices;
	unsigned int numFaces = mesh->mNumFaces;
	unsigned int numInds = numFaces * 3;

	// clear the mesh buffer and initialize it for this mesh
	meshBuffer.Release();
	if ( !meshBuffer.Init( pDevice, numVerts, numInds ) )
	{
		return false;
	}



	// map in the indices

	unsigned int* inds = meshBuffer.BeginIndexMapping( pContext );
	if ( !inds )
	{
		meshBuffer.Release();
		return false;
	}
	unsigned int idxIndex = 0;
	for ( unsigned int idxFace = 0; idxFace < numFaces; idxFace++ )
	{
		aiFace* face = &mesh->mFaces[idxFace];
		// we're a triangles only club
		if ( face->mNumIndices != 3 )
		{
			meshBuffer.Release();
			return false;
		}

		for ( unsigned int i = 0; i < 3; i++ )
		{
			inds[idxIndex++] = face->mIndices[i];
		}
	}
	meshBuffer.FinishIndexMapping( pContext );



	// map in the positions

	sVert_PN* verts = meshBuffer.BeginVertexMapping( pContext );
	if ( !verts )
	{
		meshBuffer.Release();
		return false;
	}
	for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
	{
		aiVector3D* v = &mesh->mVertices[idxVert];
		verts[idxVert].Position = cVector3( v->x, v->y, v->z );
		verts[idxVert].PositionPad = 1.f;
		aiVector3D* n = &mesh->mNormals[idxVert];
		verts[idxVert].Normal = cVector3( n->x, n->y, n->z );
		verts[idxVert].NormalPad = 1.f;
	}
	meshBuffer.FinishVertexMapping( pContext );

	return true;
}

bool cStaticMeshLoader::Load_PNTx( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, cModelLoader& loader, cRenderBuffersT<sVert_PNTx>& meshBuffer, int meshNumber)
{
	// check that the scene is loaded
	const aiScene* scene = loader.GetScene();
	if ( !scene )
	{
		return false;
	}

	// check that it has a mesh
	if ( !scene->mMeshes || !scene->mMeshes[meshNumber] )
	{
		return false;
	}

	// get the mesh
	aiMesh* mesh = scene->mMeshes[meshNumber];

	// make sure it has faces and positions and normals
	if ( !mesh->HasPositions() || !mesh->HasFaces() || !mesh->HasNormals() )
	{
		return false;
	}

	// info...
	unsigned int numVerts = mesh->mNumVertices;
	unsigned int numFaces = mesh->mNumFaces;
	unsigned int numInds = numFaces * 3;

	// clear the mesh buffer and initialize it for this mesh
	meshBuffer.Release();
	if ( !meshBuffer.Init( pDevice, numVerts, numInds ) )
	{
		return false;
	}



	// map in the indices

	unsigned int* inds = meshBuffer.BeginIndexMapping( pContext );
	if ( !inds )
	{
		meshBuffer.Release();
		return false;
	}
	unsigned int idxIndex = 0;
	for ( unsigned int idxFace = 0; idxFace < numFaces; idxFace++ )
	{
		aiFace* face = &mesh->mFaces[idxFace];
		// we're a triangles only club
		if ( face->mNumIndices != 3 )
		{
			meshBuffer.Release();
			return false;
		}

		for ( unsigned int i = 0; i < 3; i++ )
		{
			inds[idxIndex++] = face->mIndices[i];
		}
	}
	meshBuffer.FinishIndexMapping( pContext );



	// map in the vertices

	sVert_PNTx* verts = meshBuffer.BeginVertexMapping( pContext );
	if ( !verts )
	{
		meshBuffer.Release();
		return false;
	}
	for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
	{
		aiVector3D* v = &mesh->mVertices[idxVert];
		verts[idxVert].Position = cVector3( v->x, v->y, v->z );
		verts[idxVert].PositionPad = 1.f;
		aiVector3D* n = &mesh->mNormals[idxVert];
		verts[idxVert].Normal = cVector3( n->x, n->y, n->z );
		verts[idxVert].NormalPad = 1.f;
	}

	bool result = false;
	if ( mesh->HasTextureCoords(0) )
	{
		result = true;
		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			aiVector3D* t = &mesh->mTextureCoords[0][idxVert];
			verts[idxVert].TexCoord0 = cVector2( t->x, t->y );
		}
	}
	else
	{
		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			cVector3 uvw = verts[idxVert].Position;
			uvw.Normalize();
			verts[idxVert].TexCoord0.x = ( ( asin(uvw.x) / nMath::PI ) + 0.5f ) * 2.f;
			verts[idxVert].TexCoord0.y = ( ( asin(uvw.y) / nMath::PI ) + 0.5f ) * 2.f;
		}
	}

	if ( mesh->HasTextureCoords(1) )
	{
		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			aiVector3D* t = &mesh->mTextureCoords[1][idxVert];
			verts[idxVert].TexCoord1 = cVector2( t->x, t->y );
		}
	}

	meshBuffer.FinishVertexMapping( pContext );

	return result;
}


bool cStaticMeshLoader::Load_PNTxTn( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, cModelLoader& loader, cRenderBuffersT<sVert_PNTxTn>& meshBuffer, int meshNumber )

{
	// check that the scene is loaded
	const aiScene* scene = loader.GetScene();
	if ( !scene )
	{
		return false;
	}

	// check that it has a mesh
	if ( !scene->mMeshes || !scene->mMeshes[meshNumber] )
	{
		return false;
	}

	// get the mesh
	aiMesh* mesh = scene->mMeshes[meshNumber];

	// make sure it has faces and positions and normals
	if ( !mesh->HasPositions() || !mesh->HasFaces() || !mesh->HasNormals() )
	{
		return false;
	}

	// info...
	unsigned int numVerts = mesh->mNumVertices;
	unsigned int numFaces = mesh->mNumFaces;
	unsigned int numInds = numFaces * 3;

	// clear the mesh buffer and initialize it for this mesh
	meshBuffer.Release();
	if ( !meshBuffer.Init( pDevice, numVerts, numInds ) )
	{
		return false;
	}



	// map in the indices

	unsigned int* inds = meshBuffer.BeginIndexMapping( pContext );
	if ( !inds )
	{
		meshBuffer.Release();
		return false;
	}
	unsigned int idxIndex = 0;
	for ( unsigned int idxFace = 0; idxFace < numFaces; idxFace++ )
	{
		aiFace* face = &mesh->mFaces[idxFace];
		// we're a triangles only club
		if ( face->mNumIndices != 3 )
		{
			meshBuffer.Release();
			return false;
		}

		for ( unsigned int i = 0; i < 3; i++ )
		{
			inds[idxIndex++] = face->mIndices[i];
		}
	}
	meshBuffer.FinishIndexMapping( pContext );



	// map in the vertices
	sVert_PNTxTn* verts = meshBuffer.BeginVertexMapping( pContext );
	if ( !verts )
	{
		meshBuffer.Release();
		return false;
	}
	for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
	{
		aiVector3D* v = &mesh->mVertices[idxVert];
		verts[idxVert].Position = cVector3( v->x, v->y, v->z );
		verts[idxVert].PositionPad = 1.f;
		aiVector3D* n = &mesh->mNormals[idxVert];
		verts[idxVert].Normal = cVector3( n->x, n->y, n->z );
		verts[idxVert].NormalPad = 1.f;
	}

	bool result = false;
	if ( mesh->HasTextureCoords(0) )
	{
		result = true;
		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			aiVector3D* t = &mesh->mTextureCoords[0][idxVert];
			verts[idxVert].TexCoord0 = cVector2( t->x, t->y );
		}
	}
	else
	{
		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			cVector3 uvw = verts[idxVert].Normal;
			uvw.Normalize();
			verts[idxVert].TexCoord0.x = ( ( asin(uvw.x) / nMath::PI ) + 0.5f ) * 2.f;
			verts[idxVert].TexCoord0.y = ( ( asin(uvw.y) / nMath::PI ) + 0.5f ) * 2.f;
		}
	}

	if ( mesh->HasTextureCoords(1) )
	{
		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			aiVector3D* t = &mesh->mTextureCoords[1][idxVert];
			verts[idxVert].TexCoord1 = cVector2( t->x, t->y );
		}
	}

	if ( mesh->HasTangentsAndBitangents() )
	{
		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			aiVector3D* t = &mesh->mTangents[idxVert];
			verts[idxVert].Tangent = cVector3( t->x, t->y, t->z );
			verts[idxVert].TangentPad = 1.f;
		}
	}
	else
	{
		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			verts[idxVert].Tangent.Zeroes();
		}

		std::vector<int> counter( numVerts, 0 );
		for ( unsigned int idxFace = 0; idxFace < numFaces; idxFace++ )
		{
			for ( unsigned int idx = 0; idx < 3; idx++ )
			{
				unsigned int ind0 = mesh->mFaces[idxFace].mIndices[(0 + idx)%3];
				unsigned int ind1 = mesh->mFaces[idxFace].mIndices[(1 + idx)%3];
				unsigned int ind2 = mesh->mFaces[idxFace].mIndices[(2 + idx)%3];

				cVector3 alpha = verts[ind1].Position - verts[ind0].Position;
				cVector3 beta = verts[ind2].Position - verts[ind0].Position;
				cVector2 sq1 = verts[ind1].TexCoord0 - verts[ind0].TexCoord0;
				cVector2 sq2 = verts[ind2].TexCoord0 - verts[ind0].TexCoord0;

				float mulby = 1.f / ( sq1.x*sq2.y - sq2.x*sq2.y );

				verts[ind0].Tangent.x += mulby * ( sq2.y*alpha.x - sq1.y*beta.x );
				verts[ind0].Tangent.y += mulby * ( sq2.y*alpha.y - sq1.y*beta.y );
				verts[ind0].Tangent.z += mulby * ( sq2.y*alpha.z - sq1.y*beta.z );

				counter[ind0] = counter[ind0] + 1;
			}
		}

		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			verts[idxVert].Tangent /= static_cast<float>( counter[idxVert] );
			verts[idxVert].TangentPad = 1.f;
		}
	}

	meshBuffer.FinishVertexMapping( pContext );

	return result;
}

bool cStaticMeshLoader::Load_PNTxTnBt( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, cModelLoader& loader, cRenderBuffersT<sVert_PNTxTnBt>& meshBuffer )
{
	// check that the scene is loaded
	const aiScene* scene = loader.GetScene();
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

	// make sure it has faces and positions and normals
	if ( !mesh->HasPositions() || !mesh->HasFaces() || !mesh->HasNormals() )
	{
		return false;
	}

	// info...
	unsigned int numVerts = mesh->mNumVertices;
	unsigned int numFaces = mesh->mNumFaces;
	unsigned int numInds = numFaces * 3;

	// clear the mesh buffer and initialize it for this mesh
	meshBuffer.Release();
	if ( !meshBuffer.Init( pDevice, numVerts, numInds ) )
	{
		return false;
	}



	// map in the indices

	unsigned int* inds = meshBuffer.BeginIndexMapping( pContext );
	if ( !inds )
	{
		meshBuffer.Release();
		return false;
	}
	unsigned int idxIndex = 0;
	for ( unsigned int idxFace = 0; idxFace < numFaces; idxFace++ )
	{
		aiFace* face = &mesh->mFaces[idxFace];
		// we're a triangles only club
		if ( face->mNumIndices != 3 )
		{
			meshBuffer.Release();
			return false;
		}

		for ( unsigned int i = 0; i < 3; i++ )
		{
			inds[idxIndex++] = face->mIndices[i];
		}
	}
	meshBuffer.FinishIndexMapping( pContext );



	// map in the vertices

	sVert_PNTxTnBt* verts = meshBuffer.BeginVertexMapping( pContext );
	if ( !verts )
	{
		meshBuffer.Release();
		return false;
	}
	for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
	{
		aiVector3D* v = &mesh->mVertices[idxVert];
		verts[idxVert].Position = cVector3( v->x, v->y, v->z );
		verts[idxVert].PositionPad = 1.f;
		aiVector3D* n = &mesh->mNormals[idxVert];
		verts[idxVert].Normal = cVector3( n->x, n->y, n->z );
		verts[idxVert].NormalPad = 1.f;
	}

	bool result = false;
	if ( mesh->HasTextureCoords(0) )
	{
		result = true;
		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			aiVector3D* t = &mesh->mTextureCoords[0][idxVert];
			verts[idxVert].TexCoord0 = cVector2( t->x, t->y );
		}
	}
	else
	{
		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			cVector3 uvw = verts[idxVert].Normal;
			uvw.Normalize();
			verts[idxVert].TexCoord0.x = ( ( asin(uvw.x) / nMath::PI ) + 0.5f ) * 2.f;
			verts[idxVert].TexCoord0.y = ( ( asin(uvw.y) / nMath::PI ) + 0.5f ) * 2.f;
		}
	}

	if ( mesh->HasTextureCoords(1) )
	{
		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			aiVector3D* t = &mesh->mTextureCoords[1][idxVert];
			verts[idxVert].TexCoord1 = cVector2( t->x, t->y );
		}
	}

	if ( mesh->HasTangentsAndBitangents() )
	{
		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			aiVector3D* t = &mesh->mTangents[idxVert];
			verts[idxVert].Tangent = cVector3( t->x, t->y, t->z );
			verts[idxVert].TangentPad = 1.f;

			aiVector3D* b = &mesh->mBitangents[idxVert];
			verts[idxVert].Bitangent = cVector3( b->x, b->y, b->z );
			verts[idxVert].BitangentPad = 1.f;
		}
	}
	else
	{
		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			verts[idxVert].Tangent.Zeroes();
			verts[idxVert].Bitangent.Zeroes();
		}

		std::vector<int> counter( numVerts, 0 );
		for ( unsigned int idxFace = 0; idxFace < numFaces; idxFace++ )
		{
			for ( unsigned int idx = 0; idx < 3; idx++ )
			{
				unsigned int ind0 = mesh->mFaces[idxFace].mIndices[(0 + idx)%3];
				unsigned int ind1 = mesh->mFaces[idxFace].mIndices[(1 + idx)%3];
				unsigned int ind2 = mesh->mFaces[idxFace].mIndices[(2 + idx)%3];

				cVector3 alpha = verts[ind1].Position - verts[ind0].Position;
				cVector3 beta = verts[ind2].Position - verts[ind0].Position;
				cVector2 sq1 = verts[ind1].TexCoord0 - verts[ind0].TexCoord0;
				cVector2 sq2 = verts[ind2].TexCoord0 - verts[ind0].TexCoord0;

				float mulby = 1.f / ( sq1.x*sq2.y - sq2.x*sq2.y );

				verts[ind0].Tangent.x += mulby * ( sq2.y*alpha.x - sq1.y*beta.x );
				verts[ind0].Tangent.y += mulby * ( sq2.y*alpha.y - sq1.y*beta.y );
				verts[ind0].Tangent.z += mulby * ( sq2.y*alpha.z - sq1.y*beta.z );

				verts[ind0].Bitangent.x += mulby * ( -sq2.x*alpha.x + sq1.x*beta.x );
				verts[ind0].Bitangent.y += mulby * ( -sq2.x*alpha.y + sq1.x*beta.y );
				verts[ind0].Bitangent.z += mulby * ( -sq2.x*alpha.z + sq1.x*beta.z );

				counter[ind0] = counter[ind0] + 1;
			}
		}

		for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
		{
			verts[idxVert].Tangent /= static_cast<float>( counter[idxVert] );
			verts[idxVert].TangentPad = 1.f;
			verts[idxVert].Bitangent /= static_cast<float>( counter[idxVert] );
			verts[idxVert].BitangentPad = 1.f;
		}
	}

	meshBuffer.FinishVertexMapping( pContext );

	return result;
}

bool cStaticMeshLoader::Generate_TangentsAndBitangents( ID3D11DeviceContext* pContext, cRenderBuffersT<sVert_PNTxTnBt>& meshBuffer )
{
	sVert_PNTxTnBt* verts = meshBuffer.BeginVertexMapping( pContext );
	if ( !verts ) return false;
	
	unsigned int* inds = meshBuffer.BeginIndexMapping( pContext );
	if ( !inds ) return false;
	
	unsigned int numVerts = meshBuffer.GetNumVertices();
	unsigned int numFaces = meshBuffer.GetNumIndices() / 3;
	unsigned int numIndices = meshBuffer.GetNumIndices();

	for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
	{
		verts[idxVert].Tangent.Zeroes();
		verts[idxVert].Bitangent.Zeroes();
	}

	std::vector<int> counter( numVerts, 0 );
	unsigned int idxIndex = 0;
	for ( unsigned int idxFace = 0; idxFace < numFaces; idxFace++ )
	{
		idxIndex = idxFace * 3;
		for ( unsigned int idx = 0; idx < 3; idx++ )
		{

			unsigned int ind0 = inds[idxIndex + ((0 + idx)%3)];
			unsigned int ind1 = inds[idxIndex + ((1 + idx)%3)];
			unsigned int ind2 = inds[idxIndex + ((2 + idx)%3)];
			if ( ind0 >= numVerts || ind1 >= numVerts || ind2 >= numVerts )
			{
				bool sexybreakpoint = true;
			}
			cVector3 alpha = verts[ind1].Position - verts[ind0].Position;
			cVector3 beta = verts[ind2].Position - verts[ind0].Position;
			cVector2 sq1 = verts[ind1].TexCoord0 - verts[ind0].TexCoord0;
			cVector2 sq2 = verts[ind2].TexCoord0 - verts[ind0].TexCoord0;

			float mulby = 1.f / ( sq1.x*sq2.y - sq2.x*sq1.y );

			verts[ind0].Tangent.x += mulby * ( sq2.y*alpha.x - sq1.y*beta.x );
			verts[ind0].Tangent.y += mulby * ( sq2.y*alpha.y - sq1.y*beta.y );
			verts[ind0].Tangent.z += mulby * ( sq2.y*alpha.z - sq1.y*beta.z );

			verts[ind0].Bitangent.x += mulby * ( -sq2.x*alpha.x + sq1.x*beta.x );
			verts[ind0].Bitangent.y += mulby * ( -sq2.x*alpha.y + sq1.x*beta.y );
			verts[ind0].Bitangent.z += mulby * ( -sq2.x*alpha.z + sq1.x*beta.z );

			counter[ind0] = counter[ind0] + 1;
		}
	}

	for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
	{
		verts[idxVert].Tangent /= static_cast<float>( counter[idxVert] );
		verts[idxVert].TangentPad = 1.f;
		verts[idxVert].Bitangent /= static_cast<float>( counter[idxVert] );
		verts[idxVert].BitangentPad = 1.f;
	}
	
	meshBuffer.FinishVertexMapping( pContext );
	meshBuffer.FinishIndexMapping( pContext );
	return true;
}