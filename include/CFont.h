/////////////////////////////////////////
//
//   OpenLieroX
//
//   Auxiliary Software class library
//
//   based on the work of JasonB
//   enhanced by Dark Charlie and Albert Zeyer
//
//   code under LGPL
//
/////////////////////////////////////////


// Font class
// Created 15/7/01
// Jason Boettcher


#ifndef __CFONT_H__
#define __CFONT_H__

class CFont {
public:
	// Constructor
	CFont() {
		bmpFont = NULL;
		Colour = false;
		Width=18;
		OutlineFont = false;
	}


private:
	// Attributes

	SDL_Surface		*bmpFont;
	bool			Colour;
	int				Width;
	int				FontWidth[256];
	int				OutlineFont;

	// Common colours
	SDL_Surface		*bmpWhite;
	SDL_Surface		*bmpGreen;
    
    Uint32 f_pink;
    Uint32 f_blue;
    Uint32 f_white;
    Uint32 f_green;



public:
	// Methods

	int				Load(const std::string& fontname, bool _colour, int _width);
	void			CalculateWidth(void);

	void			PreCalculate(SDL_Surface *bmpSurf, Uint32 colour);

	void			Draw(SDL_Surface *dst, int x, int y, Uint32 col, char *fmt,...);
	void			Draw(SDL_Surface *dst, int x, int y, Uint32 col, const std::string& txt);
	void			DrawAdv(SDL_Surface *dst, int x, int y, int max_w, Uint32 col, char *fmt,...);
	void			DrawAdv(SDL_Surface *dst, int x, int y, int max_w, Uint32 col, const std::string& txt);
	void			DrawCentre(SDL_Surface *dst, int x, int y, Uint32 col, char *fmt,...);
	void			DrawCentre(SDL_Surface *dst, int x, int y, Uint32 col, const std::string& txt);
	void			DrawCentreAdv(SDL_Surface *dst, int x, int y, int min_x, int max_w, Uint32 col, char *fmt,...);
	void			DrawCentreAdv(SDL_Surface *dst, int x, int y, int min_x, int max_w, Uint32 col, const std::string& txt);

	void			Shutdown(void);

	void			SetOutline(int Outline);
	int				IsOutline(void);

	int				GetWidth(const std::string& buf);
	int				GetHeight(void)					{ return bmpFont->h; }
};








#endif  //  __CFONT_H__
