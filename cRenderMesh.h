#ifndef __JL_cRenderMesh_HG_20130317__
#define __JL_cRenderMesh_HG_20130317__

#include "../Assimp/assimp/Importer.hpp"
#include "../Assimp/assimp/scene.h"
#include "../Assimp/assimp/material.h"
#include "../Assimp/assimp/postprocess.h"

#include <string>
#include <vector>


#include "cRenderBuffersT.h"
#include "sVert_PNTx.h"
#include "cTexture.h"
#include "Colours.h"

/// Loads single meshes using Assimp
/// Materials are loaded too

class cRenderMesh
{
	friend class cRenderMeshLoader;

public:
	struct sMaterialDesc
	{
		sColourMaterial Material;
		std::vector<unsigned int> TexIndex;
	};

	typedef std::vector<sMaterialDesc> MaterialVector;
	typedef std::vector<sMaterialDesc>::iterator MaterialVectorI;
	typedef std::vector<cTexture> TexVector;
	typedef std::vector<cTexture>::iterator TexVectorI;

	cRenderMesh();
	virtual ~cRenderMesh();

	inline bool HasLoaded() const { return mbHasLoaded; }
	inline bool HasNormals() const { return mbHasNormals; }
	inline bool HasTexCoords() const { return mbHasTexCoords; }

	virtual void Release();
	virtual void SetContext( ID3D11DeviceContext* pContext );
	virtual void Render( ID3D11DeviceContext* pContext );

	TexVector Textures;
	MaterialVector Materials;

protected:
	bool mbHasLoaded;
	bool mbHasNormals;
	bool mbHasTexCoords;
	cRenderBuffersT<sVert_PNTx> mRenderBuffers;
};

class cRenderMeshLoader
{
	struct sInfo
	{
		bool bInDebugMode;
		std::wstring Format;
		std::wstring FileName;
		std::wstring Directory;
		std::wstring FullPath;
		float TimeToLoad;
	};
public:
	cRenderMeshLoader();
	virtual ~cRenderMeshLoader();

	bool LoadFromFile( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::wstring fileName, cRenderMesh& mesh );

private:
	virtual void LoadP( ID3D11DeviceContext* pContext, aiMesh* paiMesh, cRenderMesh& mesh );
	virtual void LoadPN( ID3D11DeviceContext* pContext, aiMesh* paiMesh, cRenderMesh& mesh );
	virtual void LoadPNTx( ID3D11DeviceContext* pContext, aiMesh* paiMesh, cRenderMesh& mesh );
	virtual void LoadPTx( ID3D11DeviceContext* pContext, aiMesh* paiMesh, cRenderMesh& mesh );
	virtual void LoadMaterials( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, aiMesh* paiMesh, cRenderMesh& mesh );
	virtual void DebugErrorMessage( std::string asciiError );

	// the assimp importer
	Assimp::Importer mImporter;
	// the currently loaded scene
	const aiScene* mScene;
	// the currently loaded scene's info
	sInfo mInfo;
};



#endif  //  __JL_cRenderMesh_HG_20130317__