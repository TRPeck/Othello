#include "cBoardSquare.h"

// TODO - comments mafucka!!!

BEGIN_EVENT_TABLE(cBoardSquare, wxButton)
	EVT_PAINT(cBoardSquare::paintEvent)
END_EVENT_TABLE()

cBoardSquare::cBoardSquare(wxWindow* parent, wxWindowID id) : wxButton(parent, id)
{
	SetMinSize(wxSize(bWidth, bHeight));
	SetSize(bWidth, bHeight);
	squareStatus = ' '; // indicates whether the square is empty or has a black or white piece
}

cBoardSquare::~cBoardSquare()
{
}

void cBoardSquare::paintEvent(wxPaintEvent& evt)
{
	wxPaintDC dc(this);
	render(dc);
}

void cBoardSquare::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void cBoardSquare::render(wxDC& dc)
{
	wxBrush brush = dc.GetBrush();
	brush.SetStyle(wxBRUSHSTYLE_SOLID);

	int width;
	int height;

	// get the width and height of the button, to use to correctly redraw the circles open resizing
	this->GetSize(&width, &height);

	if (squareStatus == 'B')
	{
		brush.SetColour(wxColour(0, 0, 0)); // black
		dc.SetBrush(brush);
		dc.DrawCircle(width/2, height/2, height/2-2);
	}
	else if (squareStatus == 'W')
	{
		brush.SetColour(wxColour(255, 255, 255)); // white
		dc.SetBrush(brush);
		dc.DrawCircle(width / 2, height / 2, height / 2-2);
	}
}

char cBoardSquare::getSquareStatus()
{
	return squareStatus;
}

void cBoardSquare::setSquareStatus(char n)
{
	squareStatus = n;
}