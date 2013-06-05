#ifndef __JL_iEffect_HG_20130402__
#define __JL_iEffect_HG_20130402__


#include <D3DX11.h>
#include <d3dx11effect.h>
#include <string>
#include <vector>
#include "Effect_Variables.h"
#include <algorithm>

class iEffect
{
public:
	typedef std::vector<iEffectVariable*>::iterator Iterator;

	class cVariableVector : protected std::vector<iEffectVariable*>
	{
		typedef std::vector<iEffectVariable*> Base;
		friend class iEffect;

	public:
		cVariableVector( eEffectVariableUsage usage ) : mUsage(usage) {}
		~cVariableVector()
		{
			for ( Iterator it = begin(); it < end(); it++ )
			{
				delete (*it);
			}
			clear();
		}

		inline iterator begin() { return Base::begin(); }
		inline iterator end() { return Base::end(); }
		inline bool empty() { return Base::empty(); }

		inline eEffectVariableUsage GetUsage() const { return mUsage; }

		inline void push_back( iEffectVariable *const &var )
		{
			Base::push_back( var );
		}

		void ResetAllVariables()
		{
			for ( Iterator it = begin(); it < end(); it++ )
			{
				(*it)->Reset();
			}
		}
	protected:
		eEffectVariableUsage mUsage;
	};


	iEffect();
	virtual ~iEffect();
			
	virtual void Release();

	inline bool IsInitialized() { return mbInitialized; }
	virtual bool LoadFXFile( std::wstring fileName, ID3D11Device* pDevice );
	
	

	bool ResetOptionalPerFrameVariablesAfterUse() const
	{
		return mbResetOptionalPerFrameAfterUse;
	}
	bool ResetOptionalPerObjectVariablesAfterUse() const
	{
		return mbResetOptionalPerObjectAfterUse;
	}
	void ResetOptionalPerFrameVariablesAfterUse( bool yesserno )
	{
		mbResetOptionalPerFrameAfterUse = yesserno;
	}
	void ResetOptionalPerObjectVariablesAfterUse( bool yesserno )
	{
		 mbResetOptionalPerObjectAfterUse = yesserno;
	}

	virtual void BeginScene( ID3D11DeviceContext* pContext ) {}
	virtual void BeginObject( ID3D11DeviceContext* pContext ) {}
	virtual void FinishObject( ID3D11DeviceContext* pContext ) {}
	virtual void FinishScene( ID3D11DeviceContext* pContext ) {}

	cVariableVector NecessaryPerFrame;
	cVariableVector OptionalPerFrame;
	cVariableVector NecessaryPerObject;
	cVariableVector OptionalPerObject;

protected:
	bool mbResetOptionalPerFrameAfterUse;
	bool mbResetOptionalPerObjectAfterUse;
	ID3DX11Effect* mEffect;
	bool mbInitialized;
	std::wstring mEffectFileName;
};

#endif  //  __JL_iEffect_HG_20130402__