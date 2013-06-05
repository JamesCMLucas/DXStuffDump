#include "cRenderMesh.h"
#include <cTimer.h>
#include "nEncoding.h"

#include <sstream>

cRenderMesh::cRenderMesh()
	: mbHasLoaded(false)
	, mbHasNormals(false)
	, mbHasTexCoords(false)
{

}

cRenderMesh::~cRenderMesh()
{
	this->Release();
}

void cRenderMesh::Release()
{
	TexVectorI itTex = Textures.begin();
	while ( itTex != Textures.end() )
	{
		itTex->Release();
		itTex++;
	}
	Textures.clear();
	Materials.clear();
	mRenderBuffers.Release();
	mbHasLoaded = false;
}

void cRenderMesh::SetContext( ID3D11DeviceContext* pContext )
{
	mRenderBuffers.SetContext( pContext );
}

void cRenderMesh::Render( ID3D11DeviceContext* pContext )
{
	mRenderBuffers.DrawIndexed( pContext, mRenderBuffers.GetNumIndices(), 0, 0 );
}


cRenderMeshLoader::cRenderMeshLoader()
	: mScene(0)
{

}

cRenderMeshLoader::~cRenderMeshLoader()
{
	
}

bool cRenderMeshLoader::LoadFromFile( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::wstring fileName, cRenderMesh& mesh )
{
	bool bInDebugMode = false;
#if defined(DEBUG) || defined(_DEBUG)
	bInDebugMode = true;
#endif

	if ( mesh.HasLoaded() )
	{
		if ( bInDebugMode )
			DebugErrorMessage( "mesh already has junk loaded into it");
	}
	mInfo.FileName = fileName;
	mInfo.Format = fileName.substr( fileName.find_last_of(L".") + 1 );
	mInfo.Directory = fileName.substr( 0, fileName.find_last_of(L"/") + 1 );
	std::string asciiFileName = nEncoding::UNICODEtoASCII( fileName );

	cTimer timer;
	mInfo.TimeToLoad = 0.f;
	timer.Restart();

	mScene = mImporter.ReadFile( asciiFileName,
		aiProcess_SortByPType |
		aiProcess_LimitBoneWeights |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenUVCoords |
		aiProcess_GenSmoothNormals |
		aiProcess_Triangulate |
		aiProcess_FindDegenerates |
		aiProcess_FindInvalidData |
		aiProcess_FixInfacingNormals |
		aiProcess_FlipUVs );

	if ( !mScene )
	{
		if ( bInDebugMode )
			DebugErrorMessage( mImporter.GetErrorString() );
		return false;
	}

	if ( mScene->mNumMeshes < 1 )
	{
		if ( bInDebugMode )
			DebugErrorMessage( "No meshes!" );
		return false;
	}
	
	// loading just one mesh
	aiMesh* paiMesh = mScene->mMeshes[0];
	
	// make sure we have positions
	if ( !paiMesh->HasPositions() )
	{
		if ( bInDebugMode )
			DebugErrorMessage( "Mesh doesn't have positions!" );
		return false;
	}

	// make sure we have positions
	if ( !paiMesh->HasFaces() )
	{
		if ( bInDebugMode )
			DebugErrorMessage( "Mesh doesn't have faces!" );
		return false;

		// check that it's all been triangulated
		for ( unsigned int idxFace = 0; idxFace < paiMesh->mNumFaces; idxFace++ )
		{
			if ( paiMesh->mFaces[idxFace].mNumIndices != 3 )
			{
				if ( bInDebugMode )
					DebugErrorMessage( "Mesh doesn't have faces!" );
				return false;
			}
		}
	}

	// initialize the render buffers in the render mesh
	if ( !mesh.mRenderBuffers.Init( pDevice, paiMesh->mNumVertices, paiMesh->mNumFaces*3 ) )
	{
		if ( bInDebugMode )
			DebugErrorMessage( "cRenderMesh render buffers didn't initialize!" );
		return false;
	}

	// load the faces
	UINT* inds = mesh.mRenderBuffers.BeginIndexMapping( pContext );
	UINT idxIndex = 0;
	for ( unsigned int idxFace = 0; idxFace < paiMesh->mNumFaces; idxFace++ )
	{
		inds[idxIndex++] = paiMesh->mFaces[idxFace].mIndices[0];
		inds[idxIndex++] = paiMesh->mFaces[idxFace].mIndices[1];
		inds[idxIndex++] = paiMesh->mFaces[idxFace].mIndices[2];
	}
	mesh.mRenderBuffers.FinishIndexMapping( pContext );

	// load the vertices
	if ( paiMesh->HasNormals() )
	{
		mesh.mbHasNormals = true;
		if ( paiMesh->HasTextureCoords(0) )
		{
			mesh.mbHasTexCoords = true;
			LoadPNTx( pContext, paiMesh, mesh );
		}
		else
		{
			mesh.mbHasTexCoords = false;
			LoadPN( pContext, paiMesh, mesh );
		}
	}
	else if ( paiMesh->HasTextureCoords(0) )
	{
		mesh.mbHasTexCoords = true;
		mesh.mbHasNormals = false;
		LoadPTx( pContext, paiMesh, mesh );
	}
	else
	{
		mesh.mbHasNormals = false;
		mesh.mbHasTexCoords = false;
		LoadP( pContext, paiMesh, mesh );
	}
	
	LoadMaterials( pDevice, pContext, paiMesh, mesh );
	
	mInfo.TimeToLoad = timer.Restart();

	return true;
}

void cRenderMeshLoader::DebugErrorMessage( std::string asciiError )
{
	std::wstring error = nEncoding::ASCIItoUNICODE( asciiError );
	MessageBox( NULL, L"cRenderMeshLoader Error!", ERROR, MB_OK );
}

void cRenderMeshLoader::LoadP( ID3D11DeviceContext* pContext, aiMesh* paiMesh, cRenderMesh& mesh )
{
	sVert_PNTx* verts = mesh.mRenderBuffers.BeginVertexMapping( pContext );
	for ( unsigned int idxVert = 0; idxVert < paiMesh->mNumVertices; idxVert++ )
	{
		aiVector3D* v = &paiMesh->mVertices[idxVert];
		verts[idxVert].Position = cVector3( v->x, v->y, v->z );
		verts[idxVert].PositionPad = 1.f;
		verts[idxVert].NormalPad = 1.f;
	}
	mesh.mRenderBuffers.FinishVertexMapping( pContext );
}

void cRenderMeshLoader::LoadPN( ID3D11DeviceContext* pContext, aiMesh* paiMesh, cRenderMesh& mesh )
{
	sVert_PNTx* verts = mesh.mRenderBuffers.BeginVertexMapping( pContext );
	for ( unsigned int idxVert = 0; idxVert < paiMesh->mNumVertices; idxVert++ )
	{
		aiVector3D* v = &paiMesh->mVertices[idxVert];
		verts[idxVert].Position = cVector3( v->x, v->y, v->z );
		verts[idxVert].PositionPad = 1.f;
		aiVector3D* n = &paiMesh->mNormals[idxVert];
		verts[idxVert].Normal = cVector3( n->x, n->y, n->z );
		verts[idxVert].NormalPad = 1.f;
	}
	mesh.mRenderBuffers.FinishVertexMapping( pContext );
}

void cRenderMeshLoader::LoadPNTx( ID3D11DeviceContext* pContext, aiMesh* paiMesh, cRenderMesh& mesh )
{
	sVert_PNTx* verts = mesh.mRenderBuffers.BeginVertexMapping( pContext );
	for ( unsigned int idxVert = 0; idxVert < paiMesh->mNumVertices; idxVert++ )
	{
		aiVector3D* v = &paiMesh->mVertices[idxVert];
		verts[idxVert].Position = cVector3( v->x, v->y, v->z );
		verts[idxVert].PositionPad = 1.f;
		aiVector3D* n = &paiMesh->mNormals[idxVert];
		verts[idxVert].Normal = cVector3( n->x, n->y, n->z );
		verts[idxVert].NormalPad = 1.f;
		aiVector3D* t = &paiMesh->mTextureCoords[0][idxVert];
		verts[idxVert].TexCoord0 = cTexCoord( t->x, t->y );
	}
	mesh.mRenderBuffers.FinishVertexMapping( pContext );
}

void cRenderMeshLoader::LoadPTx( ID3D11DeviceContext* pContext, aiMesh* paiMesh, cRenderMesh& mesh )
{
	sVert_PNTx* verts = mesh.mRenderBuffers.BeginVertexMapping( pContext );
	for ( unsigned int idxVert = 0; idxVert < paiMesh->mNumVertices; idxVert++ )
	{
		aiVector3D* v = &paiMesh->mVertices[idxVert];
		verts[idxVert].Position = cVector3( v->x, v->y, v->z );
		verts[idxVert].PositionPad = 1.f;
		verts[idxVert].NormalPad = 1.f;
		aiVector3D* t = &paiMesh->mTextureCoords[0][idxVert];
		verts[idxVert].TexCoord0 = cTexCoord( t->x, t->y );
	}
	mesh.mRenderBuffers.FinishVertexMapping( pContext );
}

void cRenderMeshLoader::LoadMaterials( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, aiMesh* paiMesh, cRenderMesh& mesh )
{
	cRenderMesh::sMaterialDesc matDesc;

	aiMaterial* mat = mScene->mMaterials[paiMesh->mMaterialIndex];
	for ( unsigned int j = 0; j < mat->mNumProperties; j++ )
	{
		aiMaterialProperty* prop = mat->mProperties[j];
		std::string key = prop->mKey.C_Str();
		switch( prop->mSemantic )
		{
		case aiTextureType_NONE:
			{
				// some non-texture type property
				// maybe a colour or something
				if ( key == "$clr.diffuse" )
				{
					// diffuse colour
					float col[4];
					mat->Get<float>( "$clr.diffuse", 0, 0, col[0] );
					matDesc.Material.Diffuse = cColour4( col[0], col[1], col[2], col[3] );
				}
				else if ( key == "$clr.ambient" )
				{
					// ambient colour
					float col[4];
					mat->Get<float>( "$clr.ambient", 0, 0, col[0] );
					matDesc.Material.Ambient = cColour4( col[0], col[1], col[2], col[3] );
				}
				else if ( key == "$clr.specular" )
				{
					// specular colour
					float col[4];
					mat->Get<float>( "$clr.specular", 0, 0, col[0] );
					matDesc.Material.Specular = cColour4( col[0], col[1], col[2], col[3] );
				}
				else if ( key == "$clr.emissive" )
				{
					// emissive colour
					float col[4];
					mat->Get<float>( "$clr.emissive", 0, 0, col[0] );
					matDesc.Material.Emissive = cColour4( col[0], col[1], col[2], col[3] );
				}
				else if ( key == "$clr.transparent" )
				{
					// colour transparency
				}
			}
			break;
		case aiTextureType_DIFFUSE:
			{
				aiString aiPath;
				aiReturn success = mat->GetTexture( aiTextureType_DIFFUSE, 0, &aiPath );
				if ( success == aiReturn_SUCCESS )
				{
					// got the texture filename... use it.
					std::wstringstream wss;
					wss << mInfo.Directory << aiPath.C_Str();
					matDesc.TexIndex.push_back( mesh.Textures.size() );
					mesh.Textures.push_back( cTexture() );
					mesh.Textures.back().SetType( E_TEXTURE_TYPE_DIFFUSE_MAP );
					if ( !mesh.Textures.back().LoadFromFile( wss.str(), pDevice ) )
					{
						matDesc.TexIndex.pop_back();
						mesh.Textures.pop_back();
						if ( mInfo.bInDebugMode )
						{
							MessageBox(NULL, L"error loading texture", wss.str().c_str(), MB_OK );
						}
					}
				}
			}
			break;
		case aiTextureType_AMBIENT:
			{
				aiString aiPath;
				aiReturn success = mat->GetTexture( aiTextureType_AMBIENT, 0, &aiPath );
				if ( success == aiReturn_SUCCESS )
				{
					std::wstringstream wss;
					wss << mInfo.Directory << aiPath.C_Str();
					matDesc.TexIndex.push_back( mesh.Textures.size() );
					mesh.Textures.push_back( cTexture() );
					mesh.Textures.back().SetType( E_TEXTURE_TYPE_AMBIENT_MAP );
					if ( !mesh.Textures.back().LoadFromFile( wss.str(), pDevice ) )
					{
						matDesc.TexIndex.pop_back();
						mesh.Textures.pop_back();
						if ( mInfo.bInDebugMode )
						{
							MessageBox(NULL, L"error loading texture", wss.str().c_str(), MB_OK );
						}
					}
				}
			}
			break;
		case aiTextureType_SPECULAR:
			{
				aiString aiPath;
				aiReturn success = mat->GetTexture( aiTextureType_SPECULAR, 0, &aiPath );
				if ( success == aiReturn_SUCCESS )
				{
					std::wstringstream wss;
					wss << mInfo.Directory << aiPath.C_Str();
					matDesc.TexIndex.push_back( mesh.Textures.size() );
					mesh.Textures.push_back( cTexture() );
					mesh.Textures.back().SetType( E_TEXTURE_TYPE_SPECULAR_MAP );
					if ( !mesh.Textures.back().LoadFromFile( wss.str(), pDevice ) )
					{
						matDesc.TexIndex.pop_back();
						mesh.Textures.pop_back();
						if ( mInfo.bInDebugMode )
						{
							MessageBox(NULL, L"error loading texture", wss.str().c_str(), MB_OK );
						}
					}
				}
			}
			break;
		case aiTextureType_EMISSIVE:
			{
				aiString aiPath;
				aiReturn success = mat->GetTexture( aiTextureType_EMISSIVE, 0, &aiPath );
				if ( success == aiReturn_SUCCESS )
				{
					std::wstringstream wss;
					wss << mInfo.Directory << aiPath.C_Str();
					matDesc.TexIndex.push_back( mesh.Textures.size() );
					mesh.Textures.push_back( cTexture() );
					mesh.Textures.back().SetType( E_TEXTURE_TYPE_EMISSIVE_MAP );
					if ( !mesh.Textures.back().LoadFromFile( wss.str(), pDevice ) )
					{
						matDesc.TexIndex.pop_back();
						mesh.Textures.pop_back();
						if ( mInfo.bInDebugMode )
						{
							MessageBox(NULL, L"error loading texture", wss.str().c_str(), MB_OK );
						}
					}
				}
			}
			break;
		case aiTextureType_HEIGHT:
			{	// bump map
				aiString aiPath;
				aiReturn success = mat->GetTexture( aiTextureType_HEIGHT, 0, &aiPath );
				if ( success == aiReturn_SUCCESS )
				{
					std::wstringstream wss;
					wss << mInfo.Directory << aiPath.C_Str();
					matDesc.TexIndex.push_back( mesh.Textures.size() );
					mesh.Textures.push_back( cTexture() );
					mesh.Textures.back().SetType( E_TEXTURE_TYPE_BUMP_MAP );
					if ( !mesh.Textures.back().LoadFromFile( wss.str(), pDevice ) )
					{
						matDesc.TexIndex.pop_back();
						mesh.Textures.pop_back();
						if ( mInfo.bInDebugMode )
						{
							MessageBox(NULL, L"error loading texture", wss.str().c_str(), MB_OK );
						}
					}
				}
			}
			break;
		default:
			break;
		}
	}
}
