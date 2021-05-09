#pragma once

#include "wx/wx.h"
#include "wx/image.h"
#include "cBoardSquare.h"
#include "cGame.h"
#include "wx/dataview.h"
#include <vector>
#include <string>
#include <sstream>

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

public:
	int nFieldWidth = 8;
	int nFieldHeight = 8;
	cBoardSquare** btn;
	char* nField = nullptr;
	char currentPlayer = 'B';
	int remainingDisks = 60;
	int scoreCount[2];
	wxColour colour1;
	wxColour colour2;
	wxDataViewListCtrl* movesList;
	wxVector<wxVariant> data;
	wxVector<wxVector<wxVariant>> allMoves;
	std::string* moves = nullptr;
	int movesListCount = 0;
	int moveCount = 0;
	wxMenuBar *menuBar = nullptr;
	int reverseCounter = 1; // get the right position for moves list to overwrite

	std::ostringstream s1; // for testing purposes (output to the debug console)

	bool legalMoves(char c);
	bool checkMove(int x, int y);
	std::vector<int> checkLeft(int x, int y);
	std::vector<int> checkRight(int x, int y);
	std::vector<int> checkUp(int x, int y);
	std::vector<int> checkDown(int x, int y);
	std::vector<int> checkUpLeft(int x, int y);
	std::vector<int> checkUpRight(int x, int y);
	std::vector<int> checkDownLeft(int x, int y);
	std::vector<int> checkDownRight(int x, int y);
	void countDisks();
	std::string getPosition(int x, int y);
	bool checkEndGame();
	void squareClicked(wxCommandEvent& evt);

private:
	cGame game;
	
	void onMenuNew(wxCommandEvent &evt);
	void onMenuOpen(wxCommandEvent &evt);
	void onMenuSave(wxCommandEvent& evt);
	void onMenuExit(wxCommandEvent& evt);

	bool open(wxString filename);
	bool save(wxString filename);

	wxDECLARE_EVENT_TABLE();
};

