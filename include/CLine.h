// OpenLieroX

// Line
// Created 5/11/06
// Dark Charlie

// code under LGPL


#ifndef __CLINE_H__
#define __CLINE_H__


// Line events
enum {
	LIN_NONE=-1
};


class CLine : public CWidget {
public:
	// Constructor
	CLine(int x1, int y1, int dx, int dy, Uint32 col) {
		iX = x1;
		iY = y1;
		iWidth = dx;
		iHeight = dy;
		iColour = col;
	}


private:
	// Attributes
	Uint32	iColour;

public:
	// Methods

	void	Create(void) { }
	void	Destroy(void) { }

	//These events return an event id, otherwise they return -1
	int		MouseOver(mouse_t *tMouse)			{ return LIN_NONE; }
	int		MouseUp(mouse_t *tMouse, int nDown)		{ return LIN_NONE; }
	int		MouseDown(mouse_t *tMouse, int nDown)	{ return LIN_NONE; }
	int		MouseWheelDown(mouse_t *tMouse)		{ return LIN_NONE; }
	int		MouseWheelUp(mouse_t *tMouse)		{ return LIN_NONE; }
	int		KeyDown(UnicodeChar c, int keysym)	{ return LIN_NONE; }
	int		KeyUp(UnicodeChar c, int keysym)	{ return LIN_NONE; }

	DWORD SendMessage(int iMsg, DWORD Param1, DWORD Param2)	{ 
							return 0;
						}
	DWORD SendMessage(int iMsg, const std::string& sStr, DWORD Param) { return 0; }
	DWORD SendMessage(int iMsg, std::string *sStr, DWORD Param)  { return 0; }

	void	ChangeColour(Uint32 col)			{ iColour = col; }

	// Draw the line
	void	Draw(SDL_Surface *bmpDest) {
		DrawLine(bmpDest, iX, iY, iX + iWidth, iY + iHeight, iColour); 
	}

	void	LoadStyle(void) {}

	static CWidget * WidgetCreator( const std::vector< CGuiSkin::WidgetVar_t > & p )
	{
		return new CLine( p[0].i, p[1].i, p[2].i, p[3].i, p[4].c  );
	};
	
	void	ProcessGuiSkinEvent(int iEvent) {};
};

static bool CLine_WidgetRegistered = 
	CGuiSkin::RegisterWidget( "line", & CLine::WidgetCreator )
							( "x", CGuiSkin::WVT_INT )
							( "y", CGuiSkin::WVT_INT )
							( "dx", CGuiSkin::WVT_INT )
							( "dy", CGuiSkin::WVT_INT )
							( "color", CGuiSkin::WVT_COLOR );

#endif  //  __CLINE_H__
