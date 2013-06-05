#ifndef __JL_cCBControllerT_HG_20130319__
#define __JL_cCBControllerT_HG_20130319__

#include "eShaderVariable.h"
#include "cDynamicBufferT.h"

template<typename CBType>
class cCBControllerT : public cDynamicBufferT<CBType>
{
public:
	typedef std::vector<nShader::eShaderVariable>				VarVector;
	typedef std::vector<nShader::eShaderVariable>::iterator	VarVectorI;

	cCBControllerT()
		: cDynamicBufferT<sCBFog>( E_BUFFER_TYPE_CB )
		, mRegisterIndex(0)
		, mbInUse(false)
	{

	}

	virtual ~cCBControllerT()
	{
		cDynamicBufferT<CBType>::Release();
	}

	/// Initialize
	virtual bool Init( ID3D11Device* pDevice, UINT numElements /* = 1 */ )
	{
		if ( !cDynamicBufferT<CBType>::Init( pDevice ) )
		{
			return false;
		}
		mbInUse = true;
		return true;
	}

	/// Set the data
	void SetData( const CBType& cbData )
	{
		mData = cbData;
	}

	/// Set the data and map in
	void SetData( const CBType& cbData, ID3D11DeviceContext* pContext )
	{
		mData = cbData;
		this->MapIn( pContext );
	}

	/// Is this buffer in use?
	inline bool InUse() const { return mbInUse; }
	/// Set if this buffer is in use
	inline void InUse( bool inUse ) { mbInUse = inUse; }
	/// Get the assigned register index
	inline UINT GetRegisterIndex() const { return mRegisterIndex; }
	/// Assign the regirster index
	inline void SetRegisterIndex( UINT index ) { mRegisterIndex = index; }
	/// Get the data assigned
	const CBType& GetData() const { return mData; }
	/// Get the data assigned
	CBType& GetData() { return mData; }

	/// Map the data from cpu to gpu
	void MapIn( ID3D11DeviceContext* pContext )
	{
		CBType* cb = cDynamicBufferT<CBType>::BeginMapping( pContext );
		*cb = mData;
		cDynamicBufferT<CBType>::FinishMapping( pContext );
	}

	/// Map the data from gpu to cpu
	void MapOut( ID3D11DeviceContext* pContext )
	{
		CBType* cb = cDynamicBufferT<CBType>::BeginMapping( pContext );
		mData = *cb;
		cDynamicBufferT<CBType>::FinishMapping( pContext );
	}

	bool HasVar( nShader::eShaderVariable var )
	{
		return mVecVarType.end() != std::find( mVecVarType.begin(), mVecVarType.end(), var );
	}

protected:
	bool AddVar( nShader::eShaderVariable var)
	{
		VarVectorI it = std::find( mVecVarType.begin(), mVecVarType.end(), var );
		if ( it == mVecVarType.end() )
		{
			mVecVarType.push_back( var );
			return true;
		}
		return false;
	}

	/// Data
	CBType	mData;
	/// Is this constant buffer in use?
	bool	mbInUse;
	/// The assigned register index
	UINT	mRegisterIndex;
private:
	VarUsageVector mVecVarType;
};

#endif  //  __JL_cCBControllerT_HG_20130319__