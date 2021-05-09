#include "cGame.h"

#include <fstream>

// TODO - comments mafucka!!!

cGame::cGame()
{
	create();
}

cGame::~cGame()
{
}

void cGame::create()
{
	nField = new char[nWidth * nHeight];
	for (int i = 0; i < nWidth * nHeight; i++)
	{
		nField[i] = ' ';
	}
	moves = new std::string[180];
	for (int i = 0; i < 180; i++)
	{
		moves[i] = " ";
	}
}

void cGame::setnField(int x, int y, char c)
{
	nField[y * nWidth + x] = c;
}

void cGame::setCurrentPlayer(char c)
{
	currentPlayer = c;
}

void cGame::setRemainingDisks(int n)
{
	remainingDisks = n;
}

void cGame::setMoveCount(int n)
{
	moveCount = n;
}

void cGame::setReverseCounter(int n)
{
	reverseCounter = n;
}

void cGame::setMovesList(int n, std::string m)
{
	moves[n] = m;
}

void cGame::setMovesListCount(int n)
{
	movesListCount = n;
}

char cGame::getnField(int x, int y)
{
	return nField[y * nWidth + x];
}

char cGame::getCurrentPlayer()
{
	return currentPlayer;
}

int cGame::getRemainingDisks()
{
	return remainingDisks;
}

int cGame::getMoveCount()
{
	return moveCount;
}

int cGame::getReverseCounter()
{
	return reverseCounter;
}

std::string cGame::getMovesList(int n)
{
	return moves[n];
}

int cGame::getMovesListCount()
{
	return movesListCount;
}

bool cGame::save(std::wstring sFile)
{
	FILE* f = nullptr;
	_wfopen_s(&f, sFile.c_str(), L"wb");
	if (f == nullptr)
		return false;

	fwrite(&currentPlayer, sizeof(char), 1, f);
	fwrite(&moveCount, sizeof(int), 1, f);
	fwrite(&remainingDisks, sizeof(int), 1, f);
	fwrite(&reverseCounter, sizeof(int), 1, f);
	fwrite(&movesListCount, sizeof(int), 1, f);
	fwrite(nField, sizeof(char), nWidth * nHeight, f);
	fwrite(moves, sizeof(std::string), 180, f);
	for (int i = 0; i < 180; i++)
	{
		s1 << moves[i];
		s1 << " ";
		OutputDebugStringA(s1.str().c_str());
		s1.str("");
	}
	// TODO - print the list, see what's happening
	fclose(f);

	return true;
}

bool cGame::load(std::wstring sFile)
{
	delete[] nField;
	delete[] moves;
	currentPlayer = ' ';
	remainingDisks = 0;
	moveCount = 0;
	reverseCounter = 0;
	movesListCount = 0;

	FILE* f = nullptr;
	_wfopen_s(&f, sFile.c_str(), L"rb");
	if (f == nullptr)
		return false;

	fread(&currentPlayer, sizeof(char), 1, f);
	fread(&moveCount, sizeof(int), 1, f);
	fread(&remainingDisks, sizeof(int), 1, f);
	fread(&reverseCounter, sizeof(int), 1, f);
	fread(&movesListCount, sizeof(int), 1, f);

	create();

	fread(nField, sizeof(char), nWidth * nHeight, f);
	fread(moves, sizeof(std::string), 180, f);
	
	fclose(f);
	
	return true;
}
