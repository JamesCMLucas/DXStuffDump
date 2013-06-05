#ifndef __JL_cModelLoader_HG_20130408__
#define __JL_cModelLoader_HG_20130408__

// assimp stuff
#include "../Assimp/assimp/Importer.hpp"
#include "../Assimp/assimp/scene.h"
#include "../Assimp/assimp/material.h"
#include "../Assimp/assimp/postprocess.h"

#include <vector>
#include <cMatrix4x4.h>

class cModelLoader
{
	struct sInfo
	{
		std::wstring Format;
		std::wstring FileName;
		std::wstring Directory;
		std::wstring FullPath;
		float TimeToLoad;
	};
public:
	struct sFaceData
	{
		std::vector<unsigned int> Indices;
	};
	class cMeshData
	{
	public:
		cMeshData() {}
		~cMeshData() {}

		std::vector<sFaceData> Faces;
		std::vector<cVector3> Positions;
		std::vector<cVector3> Normals;
		std::vector<cVector3> Tangents;
		std::vector<cColour4> Colours;
	};

	class cModelData
	{
	public:
		cModelData()
		{

		}
		~cModelData()
		{

		}
		
	};

	cModelLoader();
	virtual ~cModelLoader();
	bool Load( std::wstring fileName );
	bool Load( std::wstring fileName, std::wstring& error );

	const aiScene* GetScene() const { return mScene; }

private:
	Assimp::Importer mImporter;
	const aiScene* mScene;
	sInfo mInfo;
};

#endif  //  __JL_cModelLoader_HG_20130408__