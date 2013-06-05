#ifndef __JL_Vertices_HG_20130317__
#define __JL_Vertices_HG_20130317__

#include <D3D11.h>
#include <string>
#include <vector>


// include all the types of vertices
#include "sVert_P.h"
#include "sVert_PN.h"
#include "sVert_PNTx.h"
#include "sVert_PNTxBn.h"
#include "sVert_PNTxTn.h"
#include "sVert_PNTxTnBt.h"


namespace nVertex
{
	/// general types of vertices
	enum eVertexType
	{
		E_VERTYPE_P,
		E_VERTYPE_PN,
		E_VERTYPE_PNTX,
		E_VERTYPE_PNTXBN
	};


	const std::string POSITION = "POSITION";  // stored in cVector4
	const D3D11_INPUT_ELEMENT_DESC POSITION_DESC = 
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	
	const std::string TEXCOORD = "TEXCOORD";  // stored in cVector2
	const D3D11_INPUT_ELEMENT_DESC TEXCOORD_DESC =
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	const std::string TEXCOORD3D = "TEXCOORD3D";  // stored in cVector4
	const D3D11_INPUT_ELEMENT_DESC TEXCOORD3D_DESC =
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	const std::string NORMAL = "NORMAL";  // stored in cVector4
	const D3D11_INPUT_ELEMENT_DESC NORMAL_DESC =
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	const std::string TANGENT = "TANGENT";  // stored in cVector4
	const D3D11_INPUT_ELEMENT_DESC TANGENT_DESC =
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	const std::string BINORMAL = "BINORMAL";  // stored in cVector4
	const D3D11_INPUT_ELEMENT_DESC BINORMAL_DESC =
	{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	const std::string COLOR = "COLOR";  // stored in cVector4
	const D3D11_INPUT_ELEMENT_DESC COLOR_DESC =
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	const std::string BLENDINDICES = "BLENDINDICES";  // stored in cUint4
	const D3D11_INPUT_ELEMENT_DESC BLENDINDICES_DESC =
	{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	
	const std::string BLENDWEIGHT = "BLENDWEIGHT";  // stored in cVector4
	const D3D11_INPUT_ELEMENT_DESC BLENDWEIGHT_DESC =
	{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	/// N: number of elements
	template <unsigned int N>
	bool BuildInputElementDesc( std::vector<std::string> vecSemantics, D3D11_INPUT_ELEMENT_DESC* desc )
	{
		if ( vecSemantics.size() != N || !desc )
		{
			return false;
		}
		
		UINT numPositions = 0;
		UINT numTexCoords = 0;
		UINT numNormals = 0;
		UINT numTangents = 0;
		UINT numBinormals = 0;
		UINT numColours = 0;
		UINT numBlendIndices = 0;
		UINT numBlendWeights = 0;

		UINT alignmentOffset = 0;

		for ( unsigned int idxSemantic = 0; idxSemantic < N; idxSemantic++ )
		{
			if ( vecSemantics[idxSemantic] == POSITION )
			{
				desc[idxSemantic] = POSITION_DESC;
				desc[idxSemantic].AlignedByteOffset = alignmentOffset;
				desc[idxSemantic].SemanticIndex = numPositions;
				alignmentOffset += 16;
				numPositions++;
			}
			else if ( vecSemantics[idxSemantic] == TEXCOORD )
			{
				desc[idxSemantic] = TEXCOORD_DESC;
				desc[idxSemantic].AlignedByteOffset = alignmentOffset;
				desc[idxSemantic].SemanticIndex = numTexCoords;
				alignmentOffset += 8;
				numTexCoords++;
			}
			else if ( vecSemantics[idxSemantic] == TEXCOORD3D )
			{
				desc[idxSemantic] = TEXCOORD3D_DESC;
				desc[idxSemantic].AlignedByteOffset = alignmentOffset;
				desc[idxSemantic].SemanticIndex = numTexCoords;
				alignmentOffset += 16;
				numTexCoords++;
			}
			else if ( vecSemantics[idxSemantic] == NORMAL )
			{
				desc[idxSemantic] = NORMAL_DESC;
				desc[idxSemantic].AlignedByteOffset = alignmentOffset;
				desc[idxSemantic].SemanticIndex = numNormals;
				alignmentOffset += 16;
				numNormals++;
			}
			else if ( vecSemantics[idxSemantic] == TANGENT )
			{
				desc[idxSemantic] = TANGENT_DESC;
				desc[idxSemantic].AlignedByteOffset = alignmentOffset;
				desc[idxSemantic].SemanticIndex = numTangents;
				alignmentOffset += 16;
				numTangents++;
			}
			else if ( vecSemantics[idxSemantic] == BINORMAL )
			{
				desc[idxSemantic] = BINORMAL_DESC;
				desc[idxSemantic].AlignedByteOffset = alignmentOffset;
				desc[idxSemantic].SemanticIndex = numBinormals;
				alignmentOffset += 16;
				numBinormals++;
			}
			else if ( vecSemantics[idxSemantic] == COLOR )
			{
				desc[idxSemantic] = COLOR_DESC;
				desc[idxSemantic].AlignedByteOffset = alignmentOffset;
				desc[idxSemantic].SemanticIndex = numColours;
				alignmentOffset += 16;
				numColours++;
			}
			else if ( vecSemantics[idxSemantic] == BLENDINDICES )
			{
				desc[idxSemantic] = BLENDINDICES_DESC;
				desc[idxSemantic].AlignedByteOffset = alignmentOffset;
				desc[idxSemantic].SemanticIndex = numBlendIndices;
				alignmentOffset += 16;
				numBlendIndices++;
			}
			else if ( vecSemantics[idxSemantic] == BLENDWEIGHT )
			{
				desc[idxSemantic] = BLENDWEIGHT_DESC;
				desc[idxSemantic].AlignedByteOffset = alignmentOffset;
				desc[idxSemantic].SemanticIndex = numBlendWeights;
				alignmentOffset += 16;
				numBlendWeights++;
			}
			else
			{
				// unrecognized!
				return false;
			}
		}

		return true;
	}
};



#endif  //  __JL_Vertices_HG_20130317__