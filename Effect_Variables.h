#ifndef __JL_Effect_Variables_HG_20130402__
#define __JL_Effect_Variables_HG_20130402__

#include <D3DX11.h>
#include <d3dx11effect.h>
#include <string>
#include <vector>
#include "Basic_Lights.h"
#include <Scalar_Primitives.h>
#include <cMatrix4x4.h>



enum eEffectVariableType
{
	EFFECT_VAR_TYPE_UNKNOWN,
	EFFECT_VAR_TYPE_FLOAT1,
	EFFECT_VAR_TYPE_FLOAT2,
	EFFECT_VAR_TYPE_FLOAT3,
	EFFECT_VAR_TYPE_FLOAT4,
	EFFECT_VAR_TYPE_UINT1,
	EFFECT_VAR_TYPE_UINT2,
	EFFECT_VAR_TYPE_UINT3,
	EFFECT_VAR_TYPE_UINT4,
	EFFECT_VAR_TYPE_MATRIX,
	EFFECT_VAR_TYPE_MATRIX_VEC,
	EFFECT_VAR_TYPE_DIRECTIONAL_LIGHT,
	EFFECT_VAR_TYPE_DIRECTIONAL_LIGHT_VEC,
	EFFECT_VAR_TYPE_POINT_LIGHT,
	EFFECT_VAR_TYPE_POINT_LIGHT_VEC,
	EFFECT_VAR_TYPE_SPOT_LIGHT,
	EFFECT_VAR_TYPE_SPOT_LIGHT_VEC,
	EFFECT_VAR_TYPE_COLOUR,
	EFFECT_VAR_TYPE_COLOUR_MATERIAL,
	EFFECT_VAR_TYPE_SHADER_RESOURCE,
	EFFECT_VAR_TYPE_UNORDERED_ACCESS  // NOT IMPLEMENTED YET!!
};

enum eEffectVariableUsage
{
	EFFECT_VAR_USAGE_UNKNOWN,
	EFFECT_VAR_USAGE_NECESSARY_PER_FRAME,
	EFFECT_VAR_USAGE_OPTIONAL_PER_FRAME,
	EFFECT_VAR_USAGE_NECESSARY_PER_OBJECT,
	EFFECT_VAR_USAGE_OPTIONAL_PER_OBJECT
};

class iEffectVariable
{
public:
	iEffectVariable()
		: mType(EFFECT_VAR_TYPE_UNKNOWN)
		, mbIsSet(false)
		, mName("")
		, mSize(0)
		, mVar(0) {}
	virtual ~iEffectVariable() {}
	
	class FindByName : std::unary_function<iEffectVariable*,bool>
	{
	public:
		FindByName( std::string SearchName )
			: SearchName(SearchName) {}

		bool operator () ( iEffectVariable* &var )
		{
			return var->GetName() == SearchName;
		}

		std::string SearchName;
	};

	inline bool IsSet() const { return mbIsSet; }
	inline void Reset() { mbIsSet = false; }
	inline UINT GetSize() const { return mSize; }
	inline eEffectVariableType GetType() const { return mType; }
	inline const std::string& GetName() const { return mName; }

	virtual void Push() = 0;
	
	virtual bool SetFloat1( float val ) { return false; }
	virtual bool SetFloat2( const cVector2& val ) { return false; }
	virtual bool SetFloat3( const cVector3& val ) { return false; }
	virtual bool SetFloat4( const cVector4& val ) { return false; }
	virtual bool SetUint1( unsigned int val ) { return false; }
	virtual bool SetUint2( const cUint2& val ) { return false; }
	virtual bool SetUint3( const cUint3& val ) { return false; }
	virtual bool SetUint4( const cUint4& val ) { return false; }
	virtual bool SetMatrix( const cMatrix4x4& val ) { return false; }
	virtual bool SetMatrixVector( const std::vector<cMatrix4x4>& val ) { return false; }
	virtual bool SetDirectionalLight( const sDirectionalLight& val ) { return false; }
	virtual bool SetDirectionalLightVector( const std::vector<sDirectionalLight>& val ) { return false; }
	virtual bool SetPointLight( const sPointLight& val ) { return false; }
	virtual bool SetPointLightVector( const std::vector<sPointLight>& val ) { return false; }
	virtual bool SetSpotLight( const sSpotLight& val ) { return false; }
	virtual bool SetSpotLightVector( const std::vector<sSpotLight>& val ) { return false; }
	virtual bool SetColour( const sColour& val ) { return false; }
	virtual bool SetColourMaterial( const sColourMaterial& val ) { return false; }
	virtual bool SetShaderResource( ID3D11ShaderResourceView* val ) { return false; }

	virtual bool GetFloat1( float& val ) { return false; }
	virtual bool GetFloat2( cVector2& val ) { return false; }
	virtual bool GetFloat3( cVector3& val ) { return false; }
	virtual bool GetFloat4( cVector4& val ) { return false; }
	virtual bool GetUint1( unsigned int& val ) { return false; }
	virtual bool GetUint2( cUint2& val ) { return false; }
	virtual bool GetUint3( cUint3& val ) { return false; }
	virtual bool GetUint4( cUint4& val ) { return false; }
	virtual bool GetMatrix( cMatrix4x4& val ) { return false; }
	virtual bool GetMatrixVector( std::vector<cMatrix4x4>& val ) { return false; }
	virtual bool GetDirectionalLight( sDirectionalLight& val ) { return false; }
	virtual bool GetDirectionalLightVector( std::vector<sDirectionalLight>& val ) { return false; }
	virtual bool GetPointLight( sPointLight& val ) { return false; }
	virtual bool GetPointLightVector( std::vector<sPointLight>& val ) { return false; }
	virtual bool GetSpotLight( sSpotLight& val ) { return false; }
	virtual bool GetSpotLightVector( std::vector<sSpotLight>& val ) { return false; }
	virtual bool GetColour( sColour& val ) { return false; }
	virtual bool GetColourMaterial( sColourMaterial& val ) { return false; }
	virtual bool GetShaderResource( ID3D11ShaderResourceView* val ) { return false; }

protected:
	bool mbIsSet;
	UINT mSize;
	eEffectVariableType mType;
	std::string mName;
	ID3DX11EffectVariable* mVar;
};


template<typename T>
class iEffectVariableT : public iEffectVariable
{
public:
	iEffectVariableT() : iEffectVariable() {}
	iEffectVariableT( eEffectVariableType type, ID3DX11EffectVariable* var )
		: iEffectVariable()
	{
		mVar = var;
		mType = type;
		mSize = sizeof(T);
		D3DX11_EFFECT_VARIABLE_DESC desc;
		if ( FAILED( mVar->GetDesc( &desc ) ) )
		{
			mName = "";
		}
		else
		{
			mName = desc.Name;
		}

	}

	virtual ~iEffectVariableT()
	{

	}

	virtual void Push()
	{
		mVar->SetRawValue( &mData, 0, mSize );
	}

	virtual const T& GetData() const { return mData; }
	virtual T& GetData() { return mData; }

	virtual void UnSet()
	{
		mbIsSet = false;
	}

	virtual void SetData( const T& data )
	{
		mbIsSet = true;
		mData = data;
	}

protected:
	T mData;
};


class cFloat1EffectVariable : public iEffectVariableT<float>
{
public:
	cFloat1EffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<float>(EFFECT_VAR_TYPE_FLOAT1, var ) {}
	virtual ~cFloat1EffectVariable() {}

	virtual void Push()
	{
		mVar->AsScalar()->SetFloat( mData );
	}

	virtual bool SetFloat1( float val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetFloat1( float& val )
	{
		val = mData;
		return true;
	}
};

class cFloat2EffectVariable : public iEffectVariableT<cVector2>
{
public:
	cFloat2EffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<cVector2>(EFFECT_VAR_TYPE_FLOAT2, var ) {}
	virtual ~cFloat2EffectVariable() {}

	virtual void Push()
	{
		mVar->AsScalar()->SetFloatArray( &mData.x, 0, sizeof(cVector2) );
	}

	virtual bool SetFloat2( const cVector2& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetFloat2( cVector2& val )
	{
		val = mData;
		return true;
	}
};

class cFloat3EffectVariable : public iEffectVariableT<cVector3>
{
public:
	cFloat3EffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<cVector3>(EFFECT_VAR_TYPE_FLOAT3, var ) {}
	virtual ~cFloat3EffectVariable() {}

	virtual void Push()
	{
		mVar->AsScalar()->SetFloatArray( &mData.x, 0, sizeof(cVector3) );
	}

	virtual bool SetFloat3( const cVector3& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetFloat3( cVector3& val )
	{
		val = mData;
		return true;
	}
};

class cFloat4EffectVariable : public iEffectVariableT<cVector4>
{
public:
	cFloat4EffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<cVector4>(EFFECT_VAR_TYPE_FLOAT4, var ) {}
	virtual ~cFloat4EffectVariable() {}

	virtual void Push()
	{
		mVar->AsScalar()->SetFloatArray( &mData.x, 0, sizeof(cVector4) );
	}

	virtual bool SetFloat4( const cVector4& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetFloat4( cVector4& val )
	{
		val = mData;
		return true;
	}
};


class cUint1EffectVariable : public iEffectVariableT<unsigned int>
{
public:
	cUint1EffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<unsigned int>(EFFECT_VAR_TYPE_UINT1, var ) {}
	virtual ~cUint1EffectVariable() {}

	virtual void Push()
	{
		mVar->AsScalar()->SetInt( mData );
	}

	virtual bool SetUint1( unsigned int val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetUint1( unsigned int& val )
	{
		val = mData;
		return true;
	}
};

class cUint2EffectVariable : public iEffectVariableT<cUint2>
{
public:
	cUint2EffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<cUint2>(EFFECT_VAR_TYPE_UINT2, var ) {}
	virtual ~cUint2EffectVariable() {}

	virtual void Push()
	{
		mVar->AsScalar()->SetRawValue( &mData.x, 0, sizeof(cUint2) );
	}

	virtual bool SetUint2( const cUint2& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetUint2( cUint2& val )
	{
		val = mData;
		return true;
	}
};

class cUint3EffectVariable : public iEffectVariableT<cUint3>
{
public:
	cUint3EffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<cUint3>(EFFECT_VAR_TYPE_UINT3, var ) {}
	virtual ~cUint3EffectVariable() {}

	virtual void Push()
	{
		mVar->AsScalar()->SetRawValue( &mData.x, 0, sizeof(cUint3) );
	}

	virtual bool SetUint3( const cUint3& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetUint3( cUint3& val )
	{
		val = mData;
		return true;
	}
};

class cUint4EffectVariable : public iEffectVariableT<cUint4>
{
public:
	cUint4EffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<cUint4>(EFFECT_VAR_TYPE_UINT4, var ) {}
	virtual ~cUint4EffectVariable() {}

	virtual void Push()
	{
		mVar->AsScalar()->SetRawValue( &mData.x, 0, sizeof(cUint4) );
	}

	virtual bool SetUint4( const cUint4& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetUint4( cUint4& val )
	{
		val = mData;
		return true;
	}
};


class cMatrixEffectVariable : public iEffectVariableT<cMatrix4x4>
{
public:
	cMatrixEffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<cMatrix4x4>(EFFECT_VAR_TYPE_MATRIX, var ) {}
	virtual ~cMatrixEffectVariable() {}

	virtual void Push()
	{
		mVar->AsMatrix()->SetMatrixTranspose( mData.m );
	}

	virtual bool SetMatrix( const cMatrix4x4& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetMatrix( cMatrix4x4& val )
	{
		val = mData;
		return true;
	}
};

class cMatrixVectorEffectVariable : public iEffectVariableT<std::vector<cMatrix4x4> >
{
public:
	cMatrixVectorEffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<std::vector<cMatrix4x4> >(EFFECT_VAR_TYPE_MATRIX, var )
	{
		mSize = sizeof(cMatrix4x4);
	}

	virtual ~cMatrixVectorEffectVariable() {}

	virtual void Push()
	{
		mVar->AsMatrix()->SetMatrixTransposeArray( mData[0].m, 0, mData.size()*sizeof(cMatrix4x4) );
	}

	virtual bool SetMatrixVector( const std::vector<cMatrix4x4>& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetMatrixVector( std::vector<cMatrix4x4>& val )
	{
		val = mData;
		return true;
	}
};



class cDirectionalLightEffectVariable : public iEffectVariableT<sDirectionalLight>
{
public:
	cDirectionalLightEffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<sDirectionalLight>(EFFECT_VAR_TYPE_DIRECTIONAL_LIGHT, var ) {}
	virtual ~cDirectionalLightEffectVariable() {}

	virtual void Push()
	{
		mVar->SetRawValue( &mData, 0, sizeof(sDirectionalLight) );
	}

	virtual bool SetDirectionalLight( const sDirectionalLight& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetDirectionalLight( sDirectionalLight& val )
	{
		val = mData;
		return true;
	}
};


class cDirectionalLightVectorEffectVariable : public iEffectVariableT<std::vector<sDirectionalLight> >
{
public:
	cDirectionalLightVectorEffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<std::vector<sDirectionalLight> >(EFFECT_VAR_TYPE_DIRECTIONAL_LIGHT_VEC, var ) 
	{
		mSize = sizeof(sDirectionalLight);
	}
	virtual ~cDirectionalLightVectorEffectVariable() {}

	virtual void Push()
	{
		mVar->SetRawValue( &mData[0], 0, mData.size()*sizeof(sDirectionalLight) );
	}

	virtual bool SetDirectionalLightVector( const std::vector<sDirectionalLight>& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetDirectionalLightVector( std::vector<sDirectionalLight>& val )
	{
		val = mData;
		return true;
	}
};



class cPointLightEffectVariable : public iEffectVariableT<sPointLight>
{
public:
	cPointLightEffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<sPointLight>(EFFECT_VAR_TYPE_POINT_LIGHT, var ) {}
	virtual ~cPointLightEffectVariable() {}

	virtual void Push()
	{
		mVar->SetRawValue( &mData, 0, sizeof(sPointLight) );
	}

	virtual bool SetPointLight( const sPointLight& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetPointLight( sPointLight& val )
	{
		val = mData;
		return true;
	}
};


class cPointLightVectorEffectVariable : public iEffectVariableT<std::vector<sPointLight> >
{
public:
	cPointLightVectorEffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<std::vector<sPointLight> >(EFFECT_VAR_TYPE_POINT_LIGHT_VEC, var )
	{
		mSize = sizeof( sPointLight );
	}
	virtual ~cPointLightVectorEffectVariable() {}

	virtual void Push( ID3D11DeviceContext* pContext )
	{
		mVar->SetRawValue( &mData[0], 0, mSize*mData.size() );
	}

	virtual bool SetPointLightVector( const std::vector<sPointLight>& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetPointLightVector( std::vector<sPointLight>& val )
	{
		val = mData;
		return true;
	}
};


class cSpotLightEffectVariable : public iEffectVariableT<sSpotLight>
{
public:
	cSpotLightEffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<sSpotLight>(EFFECT_VAR_TYPE_SPOT_LIGHT, var ) {}
	virtual ~cSpotLightEffectVariable() {}

	virtual void Push()
	{
		mVar->SetRawValue( &mData, 0, sizeof(sSpotLight) );
	}

	virtual bool SetSpotLight( const sSpotLight& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetSpotLight( sSpotLight& val )
	{
		val = mData;
		return true;
	}
};


class cSpotLightVectorEffectVariable : public iEffectVariableT<std::vector<sSpotLight> >
{
public:
	cSpotLightVectorEffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<std::vector<sSpotLight> >(EFFECT_VAR_TYPE_SPOT_LIGHT_VEC, var )
	{
		mSize = sizeof( sSpotLight );
	}
	virtual ~cSpotLightVectorEffectVariable() {}

	virtual void Push()
	{
		mVar->SetRawValue( &mData[0], 0, mData.size()*sizeof(sSpotLight) );
	}

	virtual bool SetSpotLightVector( const std::vector<sSpotLight>& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetSpotLightVector( std::vector<sSpotLight>& val )
	{
		val = mData;
		return true;
	}
};


class cColourEffectVariable : public iEffectVariableT<sColour>
{
public:
	cColourEffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<sColour>(EFFECT_VAR_TYPE_COLOUR, var ) {}
	virtual ~cColourEffectVariable() {}

	virtual void Push()
	{
		mVar->SetRawValue( &mData, 0, sizeof(sColour) );
	}

	virtual bool SetColour( const sColour& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetColour( sColour& val )
	{
		val = mData;
		return true;
	}
};

class cColourMaterialEffectVariable : public iEffectVariableT<sColourMaterial>
{
public:
	cColourMaterialEffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<sColourMaterial>(EFFECT_VAR_TYPE_COLOUR_MATERIAL, var ) {}
	virtual ~cColourMaterialEffectVariable() {}

	virtual void Push()
	{
		mVar->SetRawValue( &mData, 0, sizeof(sColourMaterial) );
	}

	virtual bool SetColourMaterial( const sColourMaterial& val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetColourMaterial( sColourMaterial& val )
	{
		val = mData;
		return true;
	}
};



class cShaderResourceEffectVariable : public iEffectVariableT<ID3D11ShaderResourceView*>
{
public:
	cShaderResourceEffectVariable( ID3DX11EffectVariable* var )
		: iEffectVariableT<ID3D11ShaderResourceView*>(EFFECT_VAR_TYPE_SHADER_RESOURCE, var ) {}
	virtual ~cShaderResourceEffectVariable() {}

	virtual void Push()
	{
		mVar->AsShaderResource()->SetResource( mData );
	}

	virtual bool SetShaderResource( ID3D11ShaderResourceView* val )
	{
		mData = val;
		mbIsSet = true;
		return true;
	}

	virtual bool GetShaderResource( ID3D11ShaderResourceView* val )
	{
		val = mData;
		return true;
	}
};


namespace nShader
{
	const std::string World = "World";
	const std::string WorldInvTranspose = "WorldInvTranspose";
	const std::string View = "View";
	const std::string Projection = "Projection";
	const std::string EyePosition = "EyePosition";
	const std::string TargetPosition = "TargetPosition";
	const std::string EyeUp = "EyeUp";
};


#endif  //  __JL_Effect_Variables_HG_20130402__