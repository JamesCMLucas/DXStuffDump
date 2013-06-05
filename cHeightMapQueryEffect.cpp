#include "cHeightMapQueryEffect.h"


cHeightMapQueryEffect::cHeightMapQueryEffect()
{
	mEffectFileName = L"../Resources/Effects/PerspectiveTerrainQuery.fx";
}

cHeightMapQueryEffect::~cHeightMapQueryEffect()
{
	this->Release();
}

void cHeightMapQueryEffect::Release()
{
	Base::Release();
}

bool cHeightMapQueryEffect::Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	if ( !Base::LoadFXFile( mEffectFileName, pDevice ) )
	{
		return false;
	}

	if ( !mOutputHeights.Init( pContext, MAX_POSITIONS, 0 ) )
	{
		bool shit = true;
	}

	if ( !mInputPositions.Init( pContext, MAX_POSITIONS, 0 ) )
	{
		bool shit = true;
	}

	Tech = mEffect->GetTechniqueByName("TechComputeHeights");
	VarMapWidth = mEffect->GetVariableByName("MapWidth")->AsScalar();
	VarMapDepth = mEffect->GetVariableByName("MapDepth")->AsScalar();
	VarHeightScale = mEffect->GetVariableByName("HeightScale")->AsScalar();
	VarHeights = mEffect->GetVariableByName("Heights")->AsUnorderedAccessView();
	VarPositions = mEffect->GetVariableByName("Positions")->AsShaderResource();
	VarCoarseMapSRV = mEffect->GetVariableByName("CoarseHeightMap")->AsShaderResource();

	return true;
}

void cHeightMapQueryEffect::DoTheThing( ID3D11DeviceContext* pContext, float mapWidth, float mapDepth, float mapHeightScale, ID3D11ShaderResourceView* pMapSRV, std::vector<cVector2>& positions, std::vector<float>& outputHeights )
{
	VarHeights->SetUnorderedAccessView( mOutputHeights.pUAV );
	VarMapWidth->SetFloat( mapWidth );
	VarMapDepth->SetFloat( mapDepth );
	VarHeightScale->SetFloat( mapHeightScale );
	VarCoarseMapSRV->SetResource( pMapSRV );

	unsigned int numPositions = positions.size();
	if ( numPositions <= MAX_POSITIONS )
	{
		this->DoEEET( pContext, positions, outputHeights );
		return;
	}
	
	unsigned int numMaxIters = ( numPositions / MAX_POSITIONS );
	std::vector<float> bunnyHeights;
	std::vector<cVector2> terrainPositions( MAX_POSITIONS );
	unsigned int basePos = 0;
	outputHeights.resize( numPositions );

	for ( unsigned int idxBase = 0; idxBase < numMaxIters; idxBase++ )
	{
		for ( unsigned int i = 0; i < terrainPositions.size(); i++ )
		{
			terrainPositions[i] = positions[i + basePos];
		}
		this->DoEEET( pContext, terrainPositions, bunnyHeights );

		for ( unsigned int i = 0; i < terrainPositions.size(); i++ )
		{
			outputHeights[i + basePos] = bunnyHeights[i];
		}

		basePos += MAX_POSITIONS;
	}

	terrainPositions.resize( outputHeights.size() - basePos );
	for ( unsigned int i = 0; i < terrainPositions.size(); i++ )
	{
		terrainPositions[i] = positions[i + basePos];
	}
	this->DoEEET( pContext, terrainPositions, bunnyHeights );
	
	for ( unsigned int i = 0; i < terrainPositions.size(); i++ )
	{
		outputHeights[i + basePos] = bunnyHeights[i];
	}
	
	return;
}

void cHeightMapQueryEffect::DoEEET( ID3D11DeviceContext* pContext, std::vector<cVector2>& positions, std::vector<float>& outputHeights )
{

	unsigned int numPositions = positions.size();
	cVector2* pPos = mInputPositions.BeginInputMapping( pContext );
	for ( unsigned int idxPos = 0; idxPos < numPositions; idxPos++ )
	{
		pPos[idxPos] = positions[idxPos];
	}
	mInputPositions.FinishInputMapping( pContext );
	VarPositions->SetResource( mInputPositions.pSRV );

	Tech->GetPassByIndex(0)->Apply( 0, pContext );
	pContext->Dispatch( numPositions, 1, 1 );


	pContext->CSSetShader( 0, 0, 0 );

	outputHeights.resize(numPositions);
	float* heights = mOutputHeights.BeginOutputMapping( pContext );
	std::vector<float> heightVec;
	for ( unsigned int idxHeight = 0; idxHeight < numPositions; idxHeight++ )
	{
		outputHeights[idxHeight] = heights[idxHeight];
	}
	mOutputHeights.FinishOutputMapping( pContext );
}
