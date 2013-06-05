#include "cGameWindow.h"
#include "cGameSetupInfo.h"
#include "cMouseInput.h"

cGameWindow* cGameWindow::s_mInstance = 0;

cGameWindow* cGameWindow::GetInstance()
{
	if ( !s_mInstance )
	{
		s_mInstance = new cGameWindow();
	}
	return s_mInstance;
}

void cGameWindow::Release()
{
	if ( s_mInstance )
	{
		delete s_mInstance;
		s_mInstance = 0;
	}
}

cGameWindow::cGameWindow()
	: mGame(0)
	, mbCurrentlyResizing(false)
{
	
}

cGameWindow::~cGameWindow()
{

}

bool cGameWindow::Init( iWindowsGame* pGame, LPCWSTR className, HINSTANCE hInst, int ncmdShow )
{
	mGame = pGame;
	cGameSetupInfo setup;
	mGame->GameWindowSetup( setup );
	mGame->mWindowWidth = setup.GetWindowWidth();
	mGame->mWindowHeight = setup.GetWindowHeight();
	std::wstring windowTitle = setup.GetWindowTitle();
	mbFixedFrameRate = setup.IsFixedFrameRate();
	mFrameRate = setup.GetFrameRate();

	mHINST = hInst;
	mNCMDSHOW = ncmdShow;

	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &cGameWindow::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon( hInst, ( LPCTSTR )IDI_APPLICATION );
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_APPLICATION );

	if( !RegisterClassEx( &wcex ) )
	{
		return false;
	}
	
	this->mHWND = CreateWindow( className, windowTitle.c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, mGame->mWindowWidth, mGame->mWindowHeight, NULL, NULL, hInst, NULL );

	if ( !this->mHWND )
	{
		return false;
	}
	
	if ( !mGame->GameInit( mHWND, setup ) )
	{
		return false;
	}

	return true;
}


void cGameWindow::Run()
{
	ShowWindow( mHWND, mNCMDSHOW );

	MSG msg = {0};
	mGameTimer.Restart();
	mFrameTimer.Restart();

	while ( msg.message != WM_QUIT )
	{
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			float dt = mGameTimer.Restart();

			if ( !mGame->mbIsPaused )
			{
				//mGame->Update( dt );
				if ( mbFixedFrameRate )
				{
					if ( mFrameRate < mFrameTimer.Split() )
					{
						mGame->Update( mFrameRate );
						mGame->Draw();
						mFrameTimer.Restart();
					}
				}
				else
				{
					mGame->Update( dt );
					mGame->Draw();
				}
			}
			else
			{
				Sleep( 200 );
				
			}
		}
	}
}


LRESULT CALLBACK cGameWindow::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	return s_mInstance->GetAMessageYo( hWnd, message, wParam, lParam );
}

LRESULT cGameWindow::GetAMessageYo( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
		{
			mGame->mbIsPaused = true;
		}
		else
		{
			mGame->mbIsPaused = false;
		}
		return 0;

	case WM_SIZE:
		// Set the new window size in the game
		mGame->mWindowWidth  = LOWORD(lParam);
		mGame->mWindowHeight = HIWORD(lParam);

		if( wParam == SIZE_MINIMIZED )
		{
			mGame->mbIsPaused = true;
			mGame->mbIsMinimized = true;
			mGame->mbIsMaximized = false;
		}
		else if( wParam == SIZE_MAXIMIZED )
		{
			mGame->mbIsPaused = false;
			mGame->mbIsMinimized = false;
			mGame->mbIsMaximized = true;
			mGame->GameResize();
		}
		else if( wParam == SIZE_RESTORED )
		{

			// Restoring from minimized state?
			if( mGame->mbIsMinimized )
			{
				mGame->mbIsPaused = false;
				mGame->mbIsMinimized = false;
				mGame->GameResize();
			}

			// Restoring from maximized state?
			else if( mGame->mbIsMaximized )
			{
				mGame->mbIsPaused = false;
				mGame->mbIsMaximized = false;
				mGame->GameResize();
			}
			else if( mbCurrentlyResizing )
			{
				// If user is dragging the resize bars, we do not resize 
				// the buffers here because as the user continuously 
				// drags the resize bars, a stream of WM_SIZE messages are
				// sent to the window, and it would be pointless (and slow)
				// to resize for each WM_SIZE message received from dragging
				// the resize bars.  So instead, we reset after the user is 
				// done resizing the window and releases the resize bars, which 
				// sends a WM_EXITSIZEMOVE message.
			}
			else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
			{
				//mGame->GameResize();
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		mGame->mbIsPaused = true;
		mbCurrentlyResizing  = true;
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		mGame->mbIsPaused = false;
		mbCurrentlyResizing  = false;
		RECT rect;
		GetWindowRect( mHWND, &rect );
		mGame->mWindowWidth = static_cast<int>( rect.right - rect.left );
		mGame->mWindowHeight = static_cast<int>( rect.bottom - rect.top );
		mGame->GameResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;
	case WM_LBUTTONDOWN:
		mGame->MouseDown( E_MOUSE_BUTTON_LEFT, cMouseInput( lParam, wParam ) );
		return 0;
	case WM_MBUTTONDOWN:
		mGame->MouseDown( E_MOUSE_BUTTON_MIDDLE, cMouseInput( lParam, wParam ) );
		return 0;
	case WM_RBUTTONDOWN:
		mGame->MouseDown( E_MOUSE_BUTTON_RIGHT, cMouseInput( lParam, wParam ) );
		return 0;
	case WM_LBUTTONUP:
		mGame->MouseUp( E_MOUSE_BUTTON_LEFT, cMouseInput( lParam, wParam ) );
		return 0;
	case WM_MBUTTONUP:
		mGame->MouseUp( E_MOUSE_BUTTON_MIDDLE, cMouseInput( lParam, wParam ) );
		return 0;
	case WM_RBUTTONUP:
		mGame->MouseUp( E_MOUSE_BUTTON_RIGHT, cMouseInput( lParam, wParam ) );
		return 0;
	case WM_MOUSEMOVE:
		mGame->MouseMove( cMouseInput( lParam, wParam ) );
		return 0;
	case WM_LBUTTONDBLCLK:
		mGame->MouseDoubleClick( E_MOUSE_BUTTON_LEFT, cMouseInput( lParam, wParam ) );
		return 0;
	case WM_MBUTTONDBLCLK:
		mGame->MouseDoubleClick( E_MOUSE_BUTTON_MIDDLE, cMouseInput( lParam, wParam ) );
		return 0;
	case WM_RBUTTONDBLCLK:
		mGame->MouseDoubleClick( E_MOUSE_BUTTON_RIGHT, cMouseInput( lParam, wParam ) );
		return 0;

		// keyboard input
	case WM_KEYDOWN:
		mGame->KeyDown( static_cast<char>( wParam ) );
		return 0;
	case WM_KEYUP:
		mGame->KeyUp( static_cast<char>( wParam ) );
		return 0;

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
		break;
	}
}

HWND cGameWindow::GetHWND() const
{
	return mHWND;
}

HINSTANCE cGameWindow::GetHINSTANCE() const
{
	return mHINST;
}

int cGameWindow::GetNCMDSHOW() const
{
	return mNCMDSHOW;
}
