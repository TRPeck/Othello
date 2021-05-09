#pragma once
#include "wx/wx.h"
#include "cBoardSquare.h"
#include "wx/dataview.h"
#include <sstream>

class cGame
{
public:
	cGame();
	~cGame();

	int nWidth = 8;
	int nHeight = 8;

private:
	char* nField = nullptr;
	char currentPlayer = ' ';
	int remainingDisks = 0;
	//wxDataViewListCtrl* list;
	std::string* moves = nullptr;
	int movesListCount = 0;
	int moveCount = 0;
	int reverseCounter = 0;
	std::ostringstream s1;

	void create();

public:
	void setnField(int x, int y, char c);
	void setCurrentPlayer(char c);
	void setRemainingDisks(int n);
	void setMoveCount(int n);
	void setReverseCounter(int n);
	void setMovesList(int n, std::string m);
	void setMovesListCount(int n);
	char getnField(int x, int y);
	char getCurrentPlayer();
	int getRemainingDisks();
	int getMoveCount();
	int getReverseCounter();
	std::string getMovesList(int n);
	int getMovesListCount();

	bool save(std::wstring sFile);
	bool load(std::wstring sFile);
};

