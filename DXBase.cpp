#include "DXBase.h"




cSync::cSync()
{
	::InitializeCriticalSection(&cs);
}

cSync::~cSync()
{
	::DeleteCriticalSection(&cs);
}

void cSync::Lock()
{
	EnterCriticalSection(&cs);
}

void cSync::Unlock()
{
	LeaveCriticalSection(&cs);
}
