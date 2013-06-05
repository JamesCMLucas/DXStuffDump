#ifndef __JL_cMouseInput_HG_20130319__
#define __JL_cMouseInput_HG_20130319__

#include <Windows.h>
#include <WindowsX.h>

enum eMouseButton
{
	E_MOUSE_BUTTON_LEFT		= 0,
	E_MOUSE_BUTTON_MIDDLE	= 1,
	E_MOUSE_BUTTON_RIGHT	= 2,
	E_MOUSE_BUTTON_X1		= 4,
	E_MOUSE_BUTTON_X2		= 8,
};

class cMouseInput
{
public:
	cMouseInput( LPARAM lParam, WPARAM wParam )
		: mXPos( GET_X_LPARAM(lParam) )
		, mYPos( GET_Y_LPARAM(lParam) )
		, mbLeft( ( wParam && MK_LBUTTON ) )
		, mbMiddle( ( wParam && MK_MBUTTON ) )
		, mbRight( ( wParam && MK_RBUTTON ) )
		, mbShift( ( wParam && MK_SHIFT ) )
		, mbCtrl( ( wParam && MK_CONTROL ) )
		, mbX1( ( wParam && MK_XBUTTON1 ) )
		, mbX2( ( wParam && MK_XBUTTON2 ) ) {}
	~cMouseInput() {}

	/// Gets the x position of the mouse
	inline int GetXPos() const { return mXPos; }
	/// Gets the y position of the mouse
	inline int GetYPos() const { return mYPos; }
	/// Checks if the left mouse button is currently pressed
	inline bool IsLeftButtonPressed() const { return mbLeft; }
	/// Checks if the right mouse button is currently pressed
	inline bool IsRightButtonPressed() const { return mbRight; }
	/// Checks if the middle mouse button is currently pressed
	inline bool IsMiddleButtonPressed() const { return mbMiddle; }
	/// Checks if the shift keys is currently pressed
	inline bool IsShiftKeyPressed() const { return mbShift; }
	/// Checks if the control keys is currently pressed
	inline bool IsCtrlKeyPressed() const { return mbCtrl; }

private:
	/// The horizontal position of the mouse
	int mXPos;
	/// The vertical position of the mouse
	int mYPos;
	/// The left mouse button is down
	bool mbLeft;
	/// The middle mouse button is down
	bool mbMiddle;
	/// The right mouse button is down
	bool mbRight;
	/// The shift key is down
	bool mbShift;
	/// The control key is down
	bool mbCtrl;
	/// The first X button is down
	bool mbX1;
	/// The second X button is down
	bool mbX2;
};

#endif  //  __JL_cMouseInput_HG_20130319__