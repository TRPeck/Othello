#pragma once
#include "wx/wx.h"

class cBoardSquare : public wxButton
{
	char squareStatus;
	static const int bWidth = 75;
	static const int bHeight = 75;
public:
	cBoardSquare(wxWindow* parent, wxWindowID id);
	~cBoardSquare();

	void paintEvent(wxPaintEvent& evt);
	void paintNow();

	void render(wxDC& dc);

	char getSquareStatus();
	void setSquareStatus(char s);
	

	DECLARE_EVENT_TABLE();
};

