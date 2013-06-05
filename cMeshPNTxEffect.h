#ifndef __JL_cMeshPNTxEffect_HG_20130410__
#define __JL_cMeshPNTxEffect_HG_20130410__

#include "iEffect.h"
#include "Basic_Lights.h"


class cMesh_PNTxEffect : public iEffect
{
	typedef std::vector<sDirectionalLight> DirectionalLightVector;
	typedef std::vector<sPointLight> PointLightVector;
	typedef std::vector<sSpotLight> SpotLightVector;

public:
	DirectionalLightVector		DirectionalLights;
	PointLightVector			PointLights;
	SpotLightVector				SpotLights;

	static const UINT MAX_DIRECTIONAL_LIGHTS = 3;
	static const UINT MAX_POINT_LIGHTS = 3;
	static const UINT MAX_SPOT_LIGHTS = 3;


	cMesh_PNTxEffect()
	{

	}
	virtual ~cMesh_PNTxEffect()
	{

	}
protected:
	
private:
};

#endif  //  __JL_cMeshPNTxEffect_HG_20130410__