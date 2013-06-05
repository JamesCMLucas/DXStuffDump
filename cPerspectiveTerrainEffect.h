#ifndef __JL_cPerspectiveTerrainEffect_HG_20130408__
#define __JL_cPerspectiveTerrainEffect_HG_20130408__

#include "iEffect.h"
#include "cRenderBuffersT.h"
#include "sVert_P.h"
#include "cTexture.h"
#include "cHeightMapQueryEffect.h"

class cPerspectiveTerrainEffect : public iEffect
{
	typedef iEffect Base;
	typedef cRenderBuffersT<cVector2> Buffer;

public:
	static const unsigned int MAX_HEIGHT_QUERIES = cHeightMapQueryEffect::MAX_POSITIONS;

	cPerspectiveTerrainEffect();

	virtual ~cPerspectiveTerrainEffect();

	virtual void Release();

	float GetHeightAt( ID3D11DeviceContext* pContext, float x, float z );

	void GetHeights( ID3D11DeviceContext* pContext, std::vector<cVector2>& positions, std::vector<float>& outputHeights );

	bool Init( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::wstring plyMeshFilePath, std::wstring mapTextureFilePath, std::wstring coarseColourMapFilePath );

	void Render( ID3D11DeviceContext* pContext, const cVector3& Position, const cQuaternion& Rotation, const cMatrix4x4& proj );

	inline ID3D11ShaderResourceView* GetCoarseMapSRV() const { return mMapTex.mSRV; }
	inline float GetMapScale() const { return mHeightMapScale; }
	inline float GetMapWidth() const { return mMapWidth; }
	inline float GetMapDepth() const { return mMapDepth; }
	inline void SetMapWidth( float width ) { mMapWidth = width; }
	inline void SetMapDepth( float depth ) { mMapDepth = depth; }
	inline void SetMapWidthDepth( float width, float depth ) 
	{ 
		mMapWidth = width;
		mMapDepth = depth;
	}
	inline void SetHeightMapScale( float scale ) { mHeightMapScale = scale; }

protected:
	float mMapWidth;
	float mMapDepth;
	float mHeightMapScale;
	Buffer mMeshBuffer;
	cTexture mMapTex;
	cTexture mColourMap;
	cHeightMapQueryEffect mHeightQuery;
	

	ID3D11InputLayout*  mLayout;
	ID3DX11EffectScalarVariable* mVarCoarseHeightMapScale;
	ID3DX11EffectScalarVariable* mVarMapWidth;
	ID3DX11EffectScalarVariable* mVarMapDepth;
	ID3DX11EffectVariable* mVarWorldPositionXZ;
	ID3DX11EffectVariable* mVarLocalX;
	ID3DX11EffectVariable* mVarLocalZ;
	ID3DX11EffectMatrixVariable* mVarView;
	ID3DX11EffectMatrixVariable* mVarProjection;
	ID3DX11EffectShaderResourceVariable* mVarCoarseHeightMap;
	ID3DX11EffectShaderResourceVariable* mVarCoarseColourMap;
	ID3DX11EffectTechnique* mTech;
};

#endif  //  __JL_cPerspectiveTerrainEffect_HG_20130408__