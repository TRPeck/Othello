#pragma once

#include "wx/wx.h"
#include "wx/image.h"
#include "wx/grid.h"
#include "cBoardSquare.h"
#include "cGame.h"
#include "cSettings.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
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
	int rowCount = 0;
	std::string* moves = nullptr;
	int movesListCount = 0;
	int moveCount = 1;
	bool prevState = false;
	int latestState = 0;
	int configuration = 0;
	
	wxMenuBar* menuBar = nullptr;
	cSettings* settings = nullptr;
	
	wxPanel* infoPanel;
	wxBoxSizer* mainSizer;
	wxGrid* movesList;
	
	wxButton* firstMoveBtn;
	wxButton* prevMoveBtn;
	wxButton* nextMoveBtn;
	wxButton* lastMoveBtn;
	// REVIEW - these seem to be causing memory leaks. also do not need to be pointers. perhaps should declare here and load in constructor.
	// REVIEW - or just avoid new and pointer designation.
	wxImage* firstMoveImg = new wxImage("..\\Othello\\first_move.png", wxBITMAP_TYPE_PNG);
	wxImage* prevMoveImg = new wxImage("..\\Othello\\prev_move.png", wxBITMAP_TYPE_PNG);
	wxImage* nextMoveImg = new wxImage("..\\Othello\\next_move.png", wxBITMAP_TYPE_PNG);
	wxImage* lastMoveImg = new wxImage("..\\Othello\\last_move.png", wxBITMAP_TYPE_PNG);
	wxImage* firstMoveHoverImg = new wxImage("..\\Othello\\first_move_hover.png", wxBITMAP_TYPE_PNG);
	wxImage* prevMoveHoverImg = new wxImage("..\\Othello\\prev_move_hover.png", wxBITMAP_TYPE_PNG);
	wxImage* nextMoveHoverImg = new wxImage("..\\Othello\\next_move_hover.png", wxBITMAP_TYPE_PNG);
	wxImage* lastMoveHoverImg = new wxImage("..\\Othello\\last_move_hover.png", wxBITMAP_TYPE_PNG);
	wxImage* firstMoveClickedImg = new wxImage("..\\Othello\\first_move_clicked.png", wxBITMAP_TYPE_PNG);
	wxImage* prevMoveClickedImg = new wxImage("..\\Othello\\prev_move_clicked.png", wxBITMAP_TYPE_PNG);
	wxImage* nextMoveClickedImg = new wxImage("..\\Othello\\next_move_clicked.png", wxBITMAP_TYPE_PNG);
	wxImage* lastMoveClickedImg = new wxImage("..\\Othello\\last_move_clicked.png", wxBITMAP_TYPE_PNG);

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

	void firstMoveBtnClicked(wxCommandEvent& evt);
	void prevMoveBtnClicked(wxCommandEvent& evt);
	void nextMoveBtnClicked(wxCommandEvent& evt);
	void lastMoveBtnClicked(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();
};

