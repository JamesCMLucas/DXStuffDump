#ifndef __JL_DXBase_HG_20130411__
#define __JL_DXBase_HG_20130411__

#include<Windows.h>
#include<process.h>
#include <D3DX11.h>

/*

Misc. stuff

*/


/*! Safely release a dx component
*/
static void DXRelease( IUnknown* pUnknown )
{
	if ( pUnknown )
	{
		pUnknown->Release();
		pUnknown = 0;
	}
}


/*! A critical section lock
*/
class cSync
{
public:
	/*! The critical section
	*/
	CRITICAL_SECTION cs;
	/*! Constructor
		Creates the critical section
	*/
	cSync();
	/*! Destructor
		Deletes the critical section
	*/
	~cSync();
	/*! Enters the critical section
	*/
	void Lock();
	/*! Leaves the critical section
	*/
	void Unlock();
};


#endif  //  __JL_DXBase_HG_20130411__