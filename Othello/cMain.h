#pragma once

#include "wx/wx.h"
#include "wx/image.h"
#include "wx/grid.h"
#include "cBoardSquare.h"
#include "cGame.h"
#include "cSettings.h"
#include "wx/dataview.h"
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
	char states[180][64];
	char currentPlayer = 'B';
	int remainingDisks = 60;
	int scoreCount[2];
	bool blackLegalMoves = true;
	bool whiteLegalMoves = true;
	wxGrid* movesList;
	int rowCount = 0;
	//wxDataViewListCtrl* movesList;
	wxVector<wxVariant> data;
	wxVector<wxVector<wxVariant>> allMoves;
	std::string* moves = nullptr;
	int movesListCount = 0;
	int moveCount = 1;
	wxMenuBar *menuBar = nullptr;
	cSettings* settings = nullptr;
	int configuration = 0;
	wxPanel* infoPanel;
	bool firstResize = true;
	wxBoxSizer* mainSizer;
	
	int reverseCounter = 1; // get the right position for moves list to overwrite

	std::ostringstream s1; // for testing purposes (output to the debug console)

	int getConfiguration();
	void setConfiguration(int c);
	std::vector<int> checkLeft(int x, int y);
	std::vector<int> checkRight(int x, int y);
	std::vector<int> checkUp(int x, int y);
	std::vector<int> checkDown(int x, int y);
	std::vector<int> checkUpLeft(int x, int y);
	std::vector<int> checkUpRight(int x, int y);
	std::vector<int> checkDownLeft(int x, int y);
	std::vector<int> checkDownRight(int x, int y);
	bool legalMoves();
	std::vector<int> getLegalMoves();
	bool checkMove(int x, int y);
	void makeMove(int x, int y);
	void countDisks();
	std::string getPosition(int x, int y);
	bool checkEndGame();
	void squareClicked(wxCommandEvent& evt);
	void getState(wxGridEvent& evt);

	void weakAIMove();

private:
	cGame game;
	
	void onMenuNew(wxCommandEvent &evt);
	void onMenuOpen(wxCommandEvent &evt);
	void onMenuSave(wxCommandEvent& evt);
	void onMenuExit(wxCommandEvent& evt);
	void onMenuPSettings(wxCommandEvent& evt);

	bool open(wxString filename);
	bool save(wxString filename);

	void rangeSelect(wxMouseEvent& evt);
	void windowResized(wxSizeEvent& evt);
	void resize();

	wxDECLARE_EVENT_TABLE();
};

