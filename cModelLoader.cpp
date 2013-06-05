#pragma once
#include "cModelLoader.h"
#include "nEncoding.h"
#include <cTimer.h>
#include <Windows.h>


cModelLoader::cModelLoader()
{
	mScene = 0;
}


bool cModelLoader::Load( std::wstring fileName )
{
	std::wstring error;
	return Load( fileName, error );
}


bool cModelLoader::Load( std::wstring fileName, std::wstring& error )
{
	std::wstring directory = fileName.substr( 0, fileName.find_last_of(L"/") + 1 );
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
		//aiProcess_FixInfacingNormals |
		//aiProcess_FlipWindingOrder |
		//aiProcess_MakeLeftHanded |
		aiProcess_FlipUVs );

	if ( !mScene )
	{
		std::string asciiError = std::string( mImporter.GetErrorString() );
		error = nEncoding::ASCIItoUNICODE( asciiError );
		return false;
	}

	mInfo.TimeToLoad = timer.Restart();
	mInfo.Format = fileName.substr( fileName.find_last_of(L".") + 1 );
	mInfo.Directory = directory;
	wchar_t buf[MAX_PATH];
	GetModuleFileName( NULL, buf, MAX_PATH );
	std::wstring stringBuf = buf;
	mInfo.FullPath = stringBuf.substr( 0, stringBuf.find_last_of( L"\\/" ) );
	mInfo.FullPath += fileName;

	return true;
}

cModelLoader::~cModelLoader()
{
	
}