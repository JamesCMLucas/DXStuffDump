#ifndef __JL_cGameSetupInfo_HG_20130318__
#define __JL_cGameSetupInfo_HG_20130318__

#include <string>

struct cGameSetupInfo
{
	cGameSetupInfo()
		: WindowWidth(700), WindowHeight(500)
		, bFullScreen(false), FrameRate(1.f/60.f)
		, bFixedFrameRate(false) {}

	void SetWindowWidth( int width )
	{
		WindowWidth = width;
	}

	void SetWindowHeight( int height )
	{
		WindowHeight = height;
	}

	void SetWindowSize( int width, int height )
	{
		WindowWidth = width;
		WindowHeight = height;
	}

	void SetFullScreen( bool fullScreen )
	{
		bFullScreen = fullScreen;
	}

	void SetFrameRate( float rate )
	{
		bFixedFrameRate = true;
		FrameRate = rate;
	}

	void SetFixedFrameRate( bool fixedFrameRate )
	{
		bFixedFrameRate = fixedFrameRate;
	}

	void SetWindowTitle ( std::wstring title )
	{
		WindowTitle = title;
	}

	int GetWindowWidth() const { return WindowWidth; }
	int GetWindowHeight() const { return WindowHeight; }
	bool IsFullScreen() const { return bFullScreen; }
	bool IsFixedFrameRate() const { return bFixedFrameRate; }
	float GetFrameRate() const { return FrameRate; }
	const std::wstring& GetWindowTitle() const { return WindowTitle; }

protected:
	/// Window width
	int WindowWidth;
	/// Window height
	int WindowHeight;
	
	/// default false, overrides window height and width
	bool bFullScreen;
	/// window title caption
	std::wstring WindowTitle;
	
	/// Default 1/60
	float FrameRate;
	/// Default true, false overrides FrameRate
	bool bFixedFrameRate;
};


#endif  //  __JL_cGameSetupInfo_HG_20130318__