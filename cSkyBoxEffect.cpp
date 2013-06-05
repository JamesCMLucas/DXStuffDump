#include "cSkyBoxEffect.h"
#include <cMeshGenerator.h>

cSkyBoxEffect::cSkyBoxEffect() 
	: mTechnique(0)
	, mSRV(0)
	, mTransform(0)
	, mLayout(0)
{
	mEffectFileName = L"../Resources/Effects/SkyBox.fx";
}

cSkyBoxEffect::~cSkyBoxEffect()
{
	
}

bool cSkyBoxEffect::Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	if ( !Base::LoadFXFile( mEffectFileName, pDevice ) )
	{
		return false;
	}

	cMeshGenerator gen;
	cMeshGenerator::cMesh mesh;
	gen.CreateSphere( 10000.f, 20, 20, mesh );
	
	unsigned int numVerts = mesh.VerticesA.size();
	unsigned int numInds = mesh.Indices.size();

	if ( !mBuffers.Init( pDevice, numVerts, numInds ) )
	{
		return false;
	}
	// map the vertices
	sVert_P* verts = mBuffers.BeginVertexMapping( pContext );
	for ( unsigned int idxVert = 0; idxVert < numVerts; idxVert++ )
	{
		verts[idxVert].Position = mesh.VerticesA[idxVert].Position;
		verts[idxVert].PositionPad = 1.f;
	}
	mBuffers.FinishVertexMapping( pContext );

	// map the indices
	unsigned int* inds = mBuffers.BeginIndexMapping( pContext );
	for ( unsigned int idxIndex = 0; idxIndex < numInds; idxIndex++ )
	{
		inds[idxIndex] = mesh.Indices[idxIndex];
	}
	mBuffers.FinishIndexMapping( pContext );


	mTechnique = mEffect->GetTechniqueByIndex(0);
	if ( !mTechnique->IsValid() )
	{
		return false;
	}

	D3DX11_PASS_DESC passDesc;
	mTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	pDevice->CreateInputLayout( sVert_P::GetElementDescPtr(), sVert_P::NUM_ELEMENTS,
		passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mLayout );


	mTransform = mEffect->GetVariableByName("WorldViewProj");
	if ( mTransform->IsValid() )
	{
		NecessaryPerFrame.push_back( new cMatrixEffectVariable( mTransform ) );
	}

	ID3DX11EffectVariable* boxVar = mEffect->GetVariableByName("CubeMap");
	if ( boxVar->IsValid() )
	{
		NecessaryPerFrame.push_back( new cShaderResourceEffectVariable( boxVar ) );
	}
	mSRV = boxVar->AsShaderResource();

	return true;
}


void cSkyBoxEffect::BeginFrame( ID3D11DeviceContext* pContext )
{

}


void cSkyBoxEffect::FinishFrame( ID3D11DeviceContext* pContext )
{

}


void cSkyBoxEffect::ManualRender( ID3D11DeviceContext* pContext, const cMatrix4x4& worldViewProj, ID3D11ShaderResourceView* srv )
{
	
	mBuffers.SetContext( pContext );
	pContext->IASetInputLayout( mLayout );

	mSRV->SetResource( srv );
	mTransform->AsMatrix()->SetMatrix( worldViewProj.m );
	mTechnique->GetPassByIndex(0)->Apply( 0, pContext );
	mBuffers.DrawIndexed( pContext, mBuffers.GetNumIndices(), 0, 0 );
}
