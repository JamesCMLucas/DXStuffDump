#ifndef __JL_iWindowsGame_HG_20130318__
#define __JL_iWindowsGame_HG_20130318__

#include "cGameSetupInfo.h"
#include "cMouseInput.h"
#include <Windows.h>
#include <cTimer.h>

class iWindowsGame
{
	friend class cGameWindow;
public:
	iWindowsGame() {}
	virtual ~iWindowsGame() {}
	
	/// Get information about the game and the game window
	inline bool GameIsPaused() const { return mbIsPaused; }
	inline bool GameWindowIsMaximized() const { return mbIsMaximized; }
	inline bool GameWindowIsMinimized() const { return mbIsMinimized; }
	inline int	GetGameWindowWidth() const { return mWindowWidth; }
	inline int	GetGameWindowHeight() const { return mWindowHeight; }
	
	/// Neccessary functions to inherit from
	virtual void Update( float dt ) = 0;
	virtual void Draw() = 0;

	/// Optional functions to inherit
	virtual void GameWindowSetup( cGameSetupInfo& setup ) {}
	virtual bool GameInit( HWND hWnd, const cGameSetupInfo& setup ) { return true; }
	virtual void GameResize() {}
	virtual void GamePause() {}
	virtual void GameResume() {}
	virtual void GameQuit() {}

	/// Mouse input functions
	virtual void MouseDown( eMouseButton button, const cMouseInput& mouse ) {}
	virtual void MouseUp( eMouseButton button, const cMouseInput& mouse ) {}
	virtual void MouseMove( const cMouseInput& mouse ) {}
	virtual void MouseDoubleClick( eMouseButton button, const cMouseInput& mouse ) {}
	
	/// Keyboard input functions
	virtual void KeyDown( char key ) {}
	virtual void KeyUp( char key ) {}

private:
	int mWindowWidth;
	int mWindowHeight;
	bool mbIsMinimized;
	bool mbIsMaximized;
	bool mbIsPaused;
};

#endif  //  __JL_iWindowsGame_HG_20130318__