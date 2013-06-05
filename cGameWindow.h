#ifndef __JL_cGameWindow_HG_20130318__
#define __JL_cGameWindow_HG_20130318__

#include <Windows.h>
#include "iWindowsGame.h"
#include <cTimer.h>

class cGameWindow
{
public:
	static cGameWindow* GetInstance();
	static void Release();

	bool Init( iWindowsGame* pGame, LPCWSTR className, HINSTANCE hInst, int ncmdShow );

	static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	LRESULT GetAMessageYo( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	
	HWND GetHWND() const;
	HINSTANCE GetHINSTANCE() const;
	int GetNCMDSHOW() const;
	
	void Run();
private:
	// private constructor
	cGameWindow();
	// private destructor
	~cGameWindow();

	// disable assignment / inhabit / copy
	cGameWindow( const cGameWindow& ) {};
	cGameWindow& operator = ( const cGameWindow& ) {};
	cGameWindow( cGameWindow&& ) {}
	cGameWindow& operator = ( cGameWindow&& ) {};

	// The singleton instance
	static cGameWindow* s_mInstance;
	// The game this window is running
	iWindowsGame* mGame;
	// The game timer
	cTimer mGameTimer;
	// The frame timer
	cTimer mFrameTimer;
	// Are we sticking to a fixed frame rate?
	bool mbFixedFrameRate;
	// The frame rate to stick to (if mbFixedFrameRate == true)
	float mFrameRate;
	// Are we currently resizing the window?
	bool mbCurrentlyResizing;

	HWND mHWND;
	HINSTANCE mHINST;
	int mNCMDSHOW;
};

#define GO_GAME_GO( TheGame ) \
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ) \
{ \
	cGameWindow* theGameWindow = cGameWindow::GetInstance(); \
	iWindowsGame* theGame = new TheGame; \
	if ( !theGameWindow->Init( theGame, L"GO_GAME_GO", hInstance, nShowCmd ) ) \
	{ \
		MessageBox(NULL, L"Frak!", L"Couldn't initialize the window", MB_OK );  \
		return -1; \
	} \
	theGameWindow->Run(); \
	cGameWindow::Release(); \
	delete theGame; \
	return 0; \
}

#endif