#ifndef __JL_Static_Mesh_Loaders_HG_20130408__
#define __JL_Static_Mesh_Loaders_HG_20130408__

#include "cRenderBuffersT.h"
#include <cVector4.h>
#include "Vertices.h"

class cModelLoader;
class cStaticMeshLoader
{
public:
	static bool Load_P( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, cModelLoader& loader, cRenderBuffersT<sVert_P>& meshBuffer );
	static bool Load_PN( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, cModelLoader& loader, cRenderBuffersT<sVert_PN>& meshBuffer );
	static bool Load_PNTx( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, cModelLoader& loader, cRenderBuffersT<sVert_PNTx>& meshBuffer, int meshNumber = 0 );
	static bool Load_PNTxTn( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, cModelLoader& loader, cRenderBuffersT<sVert_PNTxTn>& meshBuffer, int meshNumber = 0 );
	static bool Load_PNTxTnBt( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, cModelLoader& loader, cRenderBuffersT<sVert_PNTxTnBt>& meshBuffer );
	static bool Generate_TangentsAndBitangents( ID3D11DeviceContext* pContext, cRenderBuffersT<sVert_PNTxTnBt>& meshBuffer );
};


#endif  //  __JL_Static_Mesh_Loaders_HG_20130408__