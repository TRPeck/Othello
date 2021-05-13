#include "cMain.h"

#include "cApp.h"

// TODO - comments mafucka!!!
// TODO - look into those memory leaks (am i cleaning up pointers properly and in the right spots?)

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_MENU(20001, cMain::onMenuNew)
	EVT_MENU(20002, cMain::onMenuSave)
	EVT_MENU(20003, cMain::onMenuOpen)
	EVT_MENU(20004, cMain::onMenuExit)
	EVT_MENU(20005, cMain::onMenuPSettings)
wxEND_EVENT_TABLE()

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "Othello", wxPoint(30, 30), wxSize(1000, 800))
{
	
	menuBar = new wxMenuBar();
	this->SetMenuBar(menuBar);
	auto menuFile = new wxMenu();
	menuFile->Append(20001, "New");
	menuFile->Append(20002, "Save");
	menuFile->Append(20003, "Open");
	menuFile->Append(20004, "Exit");
	menuBar->Append(menuFile, "File");

	auto settings = new wxMenu();
	settings->Append(20005, "Player Settings");
	menuBar->Append(settings, "Game");

	auto mainSizer = new wxBoxSizer(wxHORIZONTAL);

	// NOTE - probably should have the grid inside a panel like this. wasn't working though, possibly due to the size.
	//wxPanel* gamePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(700, 600));
	auto infoPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(300, 600));
	infoPanel->SetBackgroundColour(wxColour(100, 200, 100));
	auto infoPanelSizer = new wxBoxSizer(wxVERTICAL);

	movesList = new wxDataViewListCtrl(infoPanel, wxID_ANY, wxDefaultPosition, wxSize(275, 560));
	movesList->AppendTextColumn("#");
	movesList->AppendTextColumn("Black");
	movesList->AppendTextColumn("White");

	infoPanelSizer->Add(movesList, 1, wxEXPAND);

	btn = new cBoardSquare*[nFieldWidth * nFieldHeight];
	auto grid = new wxFlexGridSizer(nFieldWidth + 2, nFieldHeight + 2, 0, 0);
	nField = new char[nFieldWidth * nFieldHeight];
	moves = new std::string[180];

	colour1 = wxColour(213, 217, 143);
	colour2 = wxColour(128, 107, 6);

	for (int i = 0; i < nFieldWidth + 2; i++)
	{
		if (i == 0 || i == 9)
		{
			auto text = new wxStaticText(this, wxID_ANY, " ");
			grid->Add(text, wxEXPAND | wxALL);
			for (int x = 1; x < 9; x++)
			{
				int n = x + 96;
				char rank = n;
				auto text = new wxStaticText(this, wxID_ANY, rank);
				grid->Add(text, wxEXPAND | wxALL, wxALIGN_CENTRE_HORIZONTAL);
			}
			auto text2 = new wxStaticText(this, wxID_ANY, " ");
			grid->Add(text2, wxEXPAND | wxALL);
		}
		else
		{
			for (int j = 0; j < nFieldHeight + 2; j++)
			{
				if (j == 0 || j == 9)
				{
					char file = i + 48;
					auto text = new wxStaticText(this, wxID_ANY, file);
					grid->Add(text, wxEXPAND | wxALL, wxALIGN_CENTRE_VERTICAL);
				}
				else
				{
					btn[(i - 1) * nFieldWidth + (j - 1)] = new cBoardSquare(this, 10000 + ((i-1) * nFieldWidth + (j-1)));

					if (j % 2 == 0)
					{
						btn[(i - 1) * nFieldWidth + (j - 1)]->SetBackgroundColour(colour2);
					}
					else
					{
						btn[(i - 1) * nFieldWidth + (j - 1)]->SetBackgroundColour(colour1);
					}

					grid->Add(btn[(i - 1) * nFieldWidth + (j - 1)], 1, wxEXPAND | wxALL);
					btn[(i - 1) * nFieldWidth + (j - 1)]->Bind(
						wxEVT_COMMAND_BUTTON_CLICKED, &cMain::squareClicked, this);
					nField[(i - 1) * nFieldWidth + (j - 1)] = ' ';
				}
			}
			// switch the colours so the next row is opposite and continues the chessboard pattern
			wxColour temp = colour1;
			colour1 = colour2;
			colour2 = temp;
		}
	}
	// set initial four tiles in center 45 46 54 55
	btn[27]->setSquareStatus('W');
	btn[28]->setSquareStatus('B');
	btn[35]->setSquareStatus('B');
	btn[36]->setSquareStatus('W');
	btn[27]->Enable(false);
	btn[28]->Enable(false);
	btn[35]->Enable(false);
	btn[36]->Enable(false);
	nField[27] = 'W';
	nField[28] = 'B';
	nField[35] = 'B';
	nField[36] = 'W';

	infoPanel->SetSizer(infoPanelSizer);
	//gamePanel->SetSizer(grid);
	mainSizer->Add(grid, 1, wxEXPAND | wxALL, 5);
	//grid->Layout();
	mainSizer->Add(infoPanel, 0, wxEXPAND | wxTOP | wxRIGHT | wxBOTTOM, 5);
	this->SetSizerAndFit(mainSizer);
}

cMain::~cMain()
{
	delete[]btn;
	delete[]moves;
	delete[]nField;
}

int cMain::getConfiguration()
{
	return configuration;
}

void cMain::setConfiguration(int c)
{
	configuration = c;
}

// actually, i'm an idiot, java version works, this checks up. x and y axis are flipped in game development
std::vector<int> cMain::checkLeft(int x, int y)
{
	std::vector<int> flipList;
	if (y != 0)
	{
		y--; // get the next square to the left of the move
		while (y >= 0) // while we haven't reached the left side of the board
		{
			// add coordinates if they contain the other player's pieces
			if (y != 0 && nField[x * nFieldWidth + y] != currentPlayer && nField[x * nFieldWidth + y] != ' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				y--;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[x * nFieldWidth + y] == ' ')
			{
				flipList.clear();
				break;
			}
				// break and clear if reached the end of the board and its not the current player's disk 
			else if (y == 0 && nField[x * nFieldWidth + y] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[x * nFieldWidth + y] == currentPlayer)
			{
				break;
			}
		}
	}
	return flipList;
}

std::vector<int> cMain::checkRight(int x, int y)
{
	std::vector<int> flipList;
	if (y != nFieldWidth - 1)
	{
		y++; // get the next square to the left of the move
		while (y <= nFieldWidth - 1) // while we haven't reached the left side of the board
		{
			// add coordinates if they contain the other player's pieces
			if (y != nFieldWidth - 1 && nField[x * nFieldWidth + y] != currentPlayer && nField[x * nFieldWidth + y] !=
				' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				y++;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[x * nFieldWidth + y] == ' ')
			{
				flipList.clear();
				break;
			}
			else if (y == nFieldWidth - 1 && nField[x * nFieldWidth + y] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[x * nFieldWidth + y] == currentPlayer)
			{
				break;
			}
		}
	}
	return flipList;
}

std::vector<int> cMain::checkUp(int x, int y)
{
	std::vector<int> flipList;
	if (x != 0)
	{
		x--; // get the next square to the left of the move
		while (x >= 0) // while we haven't reached the left side of the board
		{
			// add coordinates if they contain the other player's pieces
			if (x != 0 && nField[x * nFieldWidth + y] != currentPlayer && nField[x * nFieldWidth + y] != ' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				x--;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[x * nFieldWidth + y] == ' ')
			{
				flipList.clear();
				break;
			}
			else if (x == 0 && nField[x * nFieldWidth + y] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[x * nFieldWidth + y] == currentPlayer)
			{
				break;
			}
		}
	}
	return flipList;
}

std::vector<int> cMain::checkDown(int x, int y)
{
	std::vector<int> flipList;
	if (x != nFieldHeight - 1)
	{
		x++; 
		while (x <= nFieldHeight - 1) 
		{
			// add coordinates if they contain the other player's pieces
			if (x != nFieldHeight - 1 && nField[x * nFieldWidth + y] != currentPlayer && nField[x * nFieldWidth + y] != ' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				x++;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[x * nFieldWidth + y] == ' ')
			{
				flipList.clear();
				break;
			}
			else if (x == nFieldHeight - 1 && nField[x * nFieldWidth + y] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[x * nFieldWidth + y] == currentPlayer)
			{
				break;
			}
		}
	}
	return flipList;
}

std::vector<int> cMain::checkUpLeft(int x, int y)
{
	std::vector<int> flipList;
	if (x != 0 && y != 0)
	{
		x--;
		y--;
		while (x >= 0 && y >= 0)
		{
			// add coordinates if they contain the other player's pieces
			if (x != 0 && y != 0 && nField[x * nFieldWidth + y] != currentPlayer && nField[x * nFieldWidth + y] != ' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				x--;
				y--;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[x * nFieldWidth + y] == ' ')
			{
				flipList.clear();
				break;
			}
			else if ((x == 0 || y == 0) && nField[x * nFieldWidth + y] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[x * nFieldWidth + y] == currentPlayer)
			{
				break;
			}
		}
	}
	return flipList;
}

std::vector<int> cMain::checkUpRight(int x, int y)
{
	std::vector<int> flipList;
	if (x != 0 && y != nFieldWidth - 1)
	{
		x--;
		y++;
		while (x >= 0 && y <= nFieldWidth - 1)
		{
			// add coordinates if they contain the other player's pieces
			if (x != 0 && y != nFieldWidth - 1 && nField[x * nFieldWidth + y] != currentPlayer && nField[x * nFieldWidth
				+ y] != ' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				x--;
				y++;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[x * nFieldWidth + y] == ' ')
			{
				flipList.clear();
				break;
			}
			else if ((x == 0 || y == nFieldWidth - 1) && nField[x * nFieldWidth + y] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[x * nFieldWidth + y] == currentPlayer)
			{
				break;
			}
		}
	}
	return flipList;
}

std::vector<int> cMain::checkDownLeft(int x, int y)
{
	std::vector<int> flipList;
	if (x != nFieldHeight - 1 && y != 0)
	{
		x++;
		y--;
		while (x <= nFieldHeight - 1 && y >= 0)
		{
			// add coordinates if they contain the other player's pieces
			if (x != nFieldHeight - 1 && y != 0 && nField[x * nFieldWidth + y] != currentPlayer && nField[x *
				nFieldWidth + y] != ' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				x++;
				y--;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[x * nFieldWidth + y] == ' ')
			{
				flipList.clear();
				break;
			}
			else if ((x == nFieldHeight - 1 || y == 0) && nField[x * nFieldWidth + y] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[x * nFieldWidth + y] == currentPlayer)
			{
				break;
			}
		}
	}
	return flipList;
}

std::vector<int> cMain::checkDownRight(int x, int y)
{
	std::vector<int> flipList;
	if (x != nFieldHeight - 1 && y != nFieldWidth - 1)
	{
		x++;
		y++;
		while (x <= nFieldHeight - 1 && y <= nFieldWidth - 1)
		{
			// add coordinates if they contain the other player's pieces
			if (x != nFieldHeight - 1 && y != nFieldWidth - 1 && nField[x * nFieldWidth + y] != currentPlayer && nField[
				x * nFieldWidth + y] != ' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				x++;
				y++;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[x * nFieldWidth + y] == ' ')
			{
				flipList.clear();
				break;
			}
			// clear and break at edge of board if that square isn't the player's
			else if ((x == nFieldHeight - 1 || y == nFieldWidth - 1) && nField[x * nFieldWidth + y] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[x * nFieldWidth + y] == currentPlayer)
			{
				break;
			}
		}
	}
	return flipList;
}

// NOTE - could probably merge these two legal moves functions
bool cMain::legalMoves()
{
	std::vector<int> flipList;
	std::vector<int> sub;
	std::vector<int> legalMoves;

	for (int x = 0; x < nFieldWidth; x++)
	{
		for (int y = 0; y < nFieldHeight; y++)
		{
			if (nField[x * nFieldWidth + y] == ' ')
			{
				/*s1 << "x * nField + y: " << x * nFieldWidth + y << " | ";
				OutputDebugStringA(s1.str().c_str());
				s1.str("");*/
				// TODO - multithreading
				sub = checkLeft(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());
				sub = checkRight(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());
				sub = checkUp(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());
				sub = checkDown(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());
				sub = checkUpLeft(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());
				sub = checkUpRight(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());
				sub = checkDownLeft(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());
				sub = checkDownRight(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());

				if (!flipList.empty())
				{
					legalMoves.push_back(x * nFieldWidth + y);
					flipList.clear();
				}
			}
		}
	}

	if (!legalMoves.empty())
	{
		/*s1 << "legal moves: ";
		for (auto i = 0; i < legalMoves.size(); i++)
		{
			s1 << legalMoves[i] << " | ";
			OutputDebugStringA(s1.str().c_str());
			s1.str("");
		}*/
		return true;
	}
	return false;
}

std::vector<int> cMain::getLegalMoves()
{
	std::vector<int> flipList;
	std::vector<int> sub;
	std::vector<int> legalMoves;

	for (int x = 0; x < nFieldWidth; x++)
	{
		for (int y = 0; y < nFieldHeight; y++)
		{
			if (nField[x * nFieldWidth + y] == ' ')
			{
				sub = checkLeft(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());
				sub = checkRight(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());
				sub = checkUp(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());
				sub = checkDown(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());
				sub = checkUpLeft(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());
				sub = checkUpRight(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());
				sub = checkDownLeft(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());
				sub = checkDownRight(x, y);
				flipList.insert(flipList.end(), sub.begin(), sub.end());

				if (!flipList.empty())
				{
					legalMoves.push_back(x * nFieldWidth + y);
					flipList.clear();
				}
			}
		}
	}

	return legalMoves;
}

bool cMain::checkMove(int x, int y)
{
	std::vector<int> flipList;
	std::vector<int> sub;
	sub = checkLeft(x, y);
	flipList.insert(flipList.end(), sub.begin(), sub.end());
	sub = checkRight(x, y);
	flipList.insert(flipList.end(), sub.begin(), sub.end());
	sub = checkUp(x, y);
	flipList.insert(flipList.end(), sub.begin(), sub.end());
	sub = checkDown(x, y);
	flipList.insert(flipList.end(), sub.begin(), sub.end());
	sub = checkUpLeft(x, y);
	flipList.insert(flipList.end(), sub.begin(), sub.end());
	sub = checkUpRight(x, y);
	flipList.insert(flipList.end(), sub.begin(), sub.end());
	sub = checkDownLeft(x, y);
	flipList.insert(flipList.end(), sub.begin(), sub.end());
	sub = checkDownRight(x, y);
	flipList.insert(flipList.end(), sub.begin(), sub.end());

	if (!flipList.empty())
	{
		/*s1 << currentPlayer << ":true ";
		OutputDebugStringA(s1.str().c_str());
		s1.str("");*/
		for (auto i = 0; i < flipList.size(); i++)
		{
			nField[flipList[i]] = currentPlayer;
			btn[flipList[i]]->setSquareStatus(currentPlayer);
			btn[flipList[i]]->Enable(false);
			s1 << "disk flipped: " << flipList[i] << " | ";
			OutputDebugStringA(s1.str().c_str());
			s1.str("");
		}
		return true;
	}
	/*s1 << currentPlayer << ":false ";
	OutputDebugStringA(s1.str().c_str());
	s1.str("");*/
	return false;
}

void cMain::makeMove(int x, int y)
{
	/*s1 << "move made: " << x * nFieldWidth + y << " | ";
	OutputDebugStringA(s1.str().c_str());
	s1.str("");*/
	// BUG - these will cause a crash if a player's turn is skipped due to attempting to pop an empty vector
	if (currentPlayer == 'B' && whiteLegalMoves)
	{
		nField[x * nFieldWidth + y] = 'B';
		btn[x * nFieldWidth + y]->paintNow();
		currentPlayer = 'W';
		btn[x * nFieldWidth + y]->setSquareStatus('B');
		remainingDisks--;
		moveCount++;
		moves[movesListCount] = std::to_string(moveCount);
		movesListCount++;
		moves[movesListCount] = getPosition(x, y);
		movesListCount++;
		std::string empty = " ";
		moves[movesListCount] = empty;
		movesListCount++;
		data.push_back(wxVariant(wxString(std::to_string(moveCount))));
		data.push_back(wxVariant(wxString(getPosition(x, y))));
		data.push_back(wxVariant(wxString(" ")));
		movesList->AppendItem(data);
		//allMoves.push_back(data);
	}
	else if (currentPlayer == 'B' && !whiteLegalMoves)
	{
		data.pop_back();
		data.push_back(wxVariant(wxString("N/A")));
		movesList->AppendItem(data);
		data.clear();
		
		nField[x * nFieldWidth + y] = 'B';
		btn[x * nFieldWidth + y]->paintNow();
		currentPlayer = 'W';
		btn[x * nFieldWidth + y]->setSquareStatus('B');
		remainingDisks--;
		moveCount++;
		moves[movesListCount] = std::to_string(moveCount);
		movesListCount++;
		moves[movesListCount] = getPosition(x, y);
		movesListCount++;
		std::string empty = " ";
		moves[movesListCount] = empty;
		movesListCount++;
		data.push_back(wxVariant(wxString(std::to_string(moveCount))));
		data.push_back(wxVariant(wxString(getPosition(x, y))));
		data.push_back(wxVariant(wxString(" ")));
		movesList->AppendItem(data);
	}
	else if (currentPlayer == 'W' && blackLegalMoves)
	{
		nField[x * nFieldWidth + y] = 'W';
		btn[x * nFieldWidth + y]->paintNow();
		currentPlayer = 'B';
		btn[x * nFieldWidth + y]->setSquareStatus('W');
		remainingDisks--;
		data.pop_back();
		data.push_back(wxVariant(wxString(getPosition(x, y))));
		moves[movesListCount - 1] = getPosition(x, y);
		//allMoves.pop_back();
		//allMoves.push_back(data);
		movesList->DeleteItem(moveCount - reverseCounter++);
		movesList->AppendItem(data);
		data.clear();
		moveCount++;
	}
	else if (currentPlayer == 'W' && !blackLegalMoves)
	{
		data.push_back(wxVariant(wxString(std::to_string(moveCount))));
		data.push_back(wxVariant(wxString("N/A")));
		
		nField[x * nFieldWidth + y] = 'W';
		btn[x * nFieldWidth + y]->paintNow();
		currentPlayer = 'B';
		btn[x * nFieldWidth + y]->setSquareStatus('W');
		remainingDisks--;
		data.push_back(wxVariant(wxString(getPosition(x, y))));
		moves[movesListCount - 1] = getPosition(x, y);
		movesList->AppendItem(data);
		data.clear();
		moveCount++;
	}
	btn[x * nFieldWidth + y]->Enable(false);
	btn[x * nFieldWidth + y]->GetParent()->Refresh();
	/*s1 << "status: " << nField[x * nFieldWidth + y] << " | ";
	OutputDebugStringA(s1.str().c_str());
	s1.str("");*/
}

void cMain::countDisks()
{
	for (int x = 0; x < nFieldWidth; x++)
	{
		for (int y = 0; y < nFieldHeight; y++)
		{
			if (nField[x * nFieldWidth + y] == 'B')
			{
				scoreCount[0]++;
			}
			else if (nField[x * nFieldWidth + y] == 'W')
			{
				scoreCount[1]++;
			}
		}
	}
}

std::string cMain::getPosition(int x, int y)
{
	std::string position;
	switch (y)
	{
	case 0:
		position = "A" + std::to_string(x + 1);
		break;
	case 1:
		position = "B" + std::to_string(x + 1);
		break;
	case 2:
		position = "C" + std::to_string(x + 1);
		break;
	case 3:
		position = "D" + std::to_string(x + 1);
		break;
	case 4:
		position = "E" + std::to_string(x + 1);
		break;
	case 5:
		position = "F" + std::to_string(x + 1);
		break;
	case 6:
		position = "G" + std::to_string(x + 1);
		break;
	case 7:
		position = "H" + std::to_string(x + 1);
		break;
	}


	return position;
}

bool cMain::checkEndGame()
{
	// if the board is full or no legal moves, the game is over
	if (remainingDisks == 0 || (!blackLegalMoves && !whiteLegalMoves))
	{
		countDisks();
		if (scoreCount[0] > scoreCount[1])
		{
			wxMessageBox("BLACK: " + std::to_string(scoreCount[0]) + " WHITE: " + std::to_string(scoreCount[1]) +
				" . BLACK WINS.");
		}
		else if (scoreCount[0] < scoreCount[1])
		{
			wxMessageBox("BLACK: " + std::to_string(scoreCount[0]) + " WHITE: " + std::to_string(scoreCount[1]) +
				" . WHITE WINS.");
		}
		else
		{
			wxMessageBox("BLACK: " + std::to_string(scoreCount[0]) + " WHITE: " + std::to_string(scoreCount[1]) +
				" . DRAW.");
		}

		// REVIEW - probably be better to not just automatically reset the game here. can't view the results this way.
		// Reset game
		/*currentPlayer = 'B';
		remainingDisks = 60;
		scoreCount[0] = 0;
		scoreCount[1] = 0;
		moveCount = 0;
		movesList->DeleteAllItems();
		allMoves.clear();
		for (int i = 0; i < nFieldWidth; i++)
		{
			for (int j = 0; j < nFieldHeight; j++)
			{
				nField[i * nFieldWidth + j] = ' ';
				btn[i * nFieldWidth + j]->setSquareStatus(' ');
				btn[i * nFieldWidth + j]->Refresh();
				btn[i * nFieldWidth + j]->Enable(true);
			}
		}
		// set initial four tiles in center 45 46 54 55
		btn[27]->setSquareStatus('W');
		btn[28]->setSquareStatus('B');
		btn[35]->setSquareStatus('B');
		btn[36]->setSquareStatus('W');
		btn[27]->Enable(false);
		btn[28]->Enable(false);
		btn[35]->Enable(false);
		btn[36]->Enable(false);
		nField[27] = 'W';
		nField[28] = 'B';
		nField[35] = 'B';
		nField[36] = 'W';*/
		return true;
	}
	return false;
}

// TODO - seems skip is used wrong. should set it to false for all of these as per the docs

void cMain::squareClicked(wxCommandEvent& evt)
{
	// check for legal move
	// set square status for current player's colour, disable
	// check surrounding square's for pieces to be flipped, change their square status and redraw
	// check endgame conditions - we'll check if there are 0 disks, rather than search the board for empty squares
	// if endgame, display results and reset
	// if not, switch the current player

	checkEndGame();

	int x = (evt.GetId() - 10000) / nFieldWidth;
	int y = (evt.GetId() - 10000) % nFieldWidth;

	/*s1 << "x: " << x << ", y: " << y;
	OutputDebugStringA(s1.str().c_str());
	s1.str("");*/

	// TODO - logic for each player1 + player2 configuration
	switch (configuration)
	{
	case 0:			// player/player
		if (legalMoves())
		{
			if (btn[x * nFieldWidth + y]->getSquareStatus() == ' ')
			{
				if (checkMove(x, y))
				{
					makeMove(x, y);
				}
			}

			// set the bools tracking whether each player has legal moves to true for the case where a move is opened up
			// by the other player's move. if this isn't the case, this code will be skipped anyway.
			if (!blackLegalMoves || !whiteLegalMoves)
			{
				blackLegalMoves = true;
				whiteLegalMoves = true;
			}
		}
		else
		{
			// TODO - set moves list appropriately for skipped turns
			wxMessageBox("No legal moves. Passing turn to other player.");
			if (currentPlayer == 'B')
			{
				currentPlayer = 'W';
				blackLegalMoves = false;
			}
			else
			{
				currentPlayer = 'B';
				whiteLegalMoves = false;
			}
		}
		break;
	case 1:			// player/weak ai
		// TODO - gotta handle this slightly different for the case where a player clicks an invalid square.
		if (currentPlayer == 'B')
		{
			if (legalMoves())
			{
				if (btn[x * nFieldWidth + y]->getSquareStatus() == ' ')
				{
					//s1 << checkMove(x, y) << " ";
					//OutputDebugStringA(s1.str().c_str());
					if (checkMove(x, y))
					{
						makeMove(x, y);
					}
				}

				if (legalMoves())
				{
					weakAIMove();

					// set the bools tracking whether each player has legal moves to true for the case where a move is opened up
					// by the other player's move. if this isn't the case, this code will be skipped anyway.
					if (!blackLegalMoves || !whiteLegalMoves)
					{
						blackLegalMoves = true;
						whiteLegalMoves = true;
					}
				}
				else
				{
					currentPlayer = 'B';
					whiteLegalMoves = false;
				}

				// set the bools tracking whether each player has legal moves to true for the case where a move is opened up
				// by the other player's move. if this isn't the case, this code will be skipped anyway.
				if (!blackLegalMoves || !whiteLegalMoves)
				{
					blackLegalMoves = true;
					whiteLegalMoves = true;
				}
			}
			else
			{
				// TODO - set moves list appropriately for skipped turns
				wxMessageBox("No legal moves. Passing turn to other player.");
				currentPlayer = 'W';
				blackLegalMoves = false;

				if (legalMoves())
				{
					weakAIMove();

					// set the bools tracking whether each player has legal moves to true for the case where a move is opened up
					// by the other player's move. if this isn't the case, this code will be skipped anyway.
					if (!blackLegalMoves || !whiteLegalMoves)
					{
						blackLegalMoves = true;
						whiteLegalMoves = true;
					}
				}
				else
				{
					currentPlayer = 'B';
					whiteLegalMoves = false;
				}
			}
		}
		break;
	case 2:			// weak ai/player
		break;
	case 3:			// weak ai/weak ai NOTE - probably don't need this since we are handling it in the other function
		break;
	}

	
	evt.Skip(false);
}

void cMain::weakAIMove()
{
	std::vector<int> movesVector = getLegalMoves();

	srand(time(NULL));

	int x = 0;
	int y = 0;

	int num = rand() % movesVector.size();
	int move = movesVector[num];
	/*for (auto i = 0; i < movesVector.size(); i++)
	{
		s1 << "move: " << movesVector[i] << " ";
		OutputDebugStringA(s1.str().c_str());
		s1.str("\n");
	}*/

	// s1 << move;
	// OutputDebugStringA(s1.str().c_str());
	// s1.str("\n");

	x = move / nFieldWidth;
	y = move % nFieldWidth;

	// s1 << "x: " << x << ", y: " << y;
	// OutputDebugStringA(s1.str().c_str());
	// s1.str("\n");

	
	/*if (btn[y * nFieldWidth + x]->getSquareStatus() == ' ')
	{
		if (checkMove(x, y))
		{
			makeMove(x, y);
		}
	}*/

	checkMove(x, y);
	makeMove(x, y);
}

void cMain::onMenuNew(wxCommandEvent& evt)
{
	// Reset game
	currentPlayer = 'B';
	remainingDisks = 60;
	scoreCount[0] = 0;
	scoreCount[1] = 0;
	moveCount = 0;
	reverseCounter = 0;
	movesListCount = 0;
	movesList->DeleteAllItems();
	// set initial four tiles in center 45 46 54 55
	for (int i = 0; i < nFieldWidth; i++)
	{
		for (int j = 0; j < nFieldHeight; j++)
		{
			nField[i * nFieldWidth + j] = ' ';
			btn[i * nFieldWidth + j]->setSquareStatus(' ');
			btn[i * nFieldWidth + j]->Refresh();
			btn[i * nFieldWidth + j]->Enable(true);
		}
	}
	btn[27]->setSquareStatus('W');
	btn[28]->setSquareStatus('B');
	btn[35]->setSquareStatus('B');
	btn[36]->setSquareStatus('W');
	btn[27]->Enable(false);
	btn[28]->Enable(false);
	btn[35]->Enable(false);
	btn[36]->Enable(false);
	btn[27]->Refresh();
	btn[28]->Refresh();
	btn[35]->Refresh();
	btn[36]->Refresh();
	nField[27] = 'W';
	nField[28] = 'B';
	nField[35] = 'B';
	nField[36] = 'W';
	evt.Skip(false);
}

void cMain::onMenuOpen(wxCommandEvent& evt)
{
	wxFileDialog dlg(this, "Open Othello Game File", "", "", ".oth Files (*.oth)|*.oth",
	                 wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (dlg.ShowModal() == wxID_OK)
	{
		//cMain* f = new cMain();
		//f->open(dlg.GetPath());
		//f->Show();
		static_cast<cMain*>(wxGetApp().getFrame())->open(dlg.GetPath());
		wxGetApp().getFrame()->Refresh();
	}
	evt.Skip(false);
}

void cMain::onMenuSave(wxCommandEvent& evt)
{
	wxFileDialog dlg(this, "Save Othello Game File", "", "", ".oth Files (*.oth)|*.oth",
	                 wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dlg.ShowModal() == wxID_OK)
	{
		static_cast<cMain*>(wxGetApp().getFrame())->save(dlg.GetPath());
	}
	evt.Skip();
}

void cMain::onMenuExit(wxCommandEvent& evt)
{
	Close();
	evt.Skip(false);
}

void cMain::onMenuPSettings(wxCommandEvent& evt)
{
	settings = new cSettings(wxT("Player Settings"), configuration);
	settings->Show();
	evt.Skip(false);

	// TODO - test this
	if (configuration == 3)
	{
		while (!checkEndGame())
		{
			if (legalMoves())
			{
				weakAIMove();

				// set the bools tracking whether each player has legal moves to true for the case where a move is opened up
				// by the other player's move. if this isn't the case, this code will be skipped anyway.
				if (!blackLegalMoves || !whiteLegalMoves)
				{
					blackLegalMoves = true;
					whiteLegalMoves = true;
				}
			}
			else
			{
				if (currentPlayer == 'B')
				{
					currentPlayer = 'W';
					blackLegalMoves = false;
				}
				else
				{
					currentPlayer = 'B';
					whiteLegalMoves = false;
				}
			}
		}
	}
}

// TODO - implement opening a saved game
// NOTE - we'll need nField, currentPlayer, movesList, moveCount, and remainingDisks
// NOTE - maybe we can just save the frame like in that one tutorial. wait, no, it isn't quite that simple
// REVIEW - opening a file that is already loaded causes a crash
bool cMain::open(wxString filename)
{
	if (!game.load(filename.wc_str()))
	{
		return false;
	}
	delete[] nField;
	delete[] moves;
	nField = new char[nFieldWidth * nFieldHeight];
	moves = new std::string[180];
	for (int i = 0; i < game.nWidth; i++)
	{
		for (int j = 0; j < game.nHeight; j++)
		{
			char cell = game.getnField(i, j);
			btn[i * nFieldWidth + j]->setSquareStatus(cell);
			nField[i * nFieldWidth + j] = cell;
		}
	}
	currentPlayer = game.getCurrentPlayer();
	remainingDisks = game.getRemainingDisks();
	moveCount = game.getMoveCount();
	reverseCounter = game.getReverseCounter();
	//movesList = new wxDataViewListCtrl();
	//movesList->AppendTextColumn("#");
	//movesList->AppendTextColumn("Black");
	//movesList->AppendTextColumn("White");
	movesList->DeleteAllItems();
	movesListCount = game.getMovesListCount();
	s1 << movesListCount;
	OutputDebugStringA(s1.str().c_str());
	s1.str("");
	for (int i = 0; i < movesListCount; i++)
	{
		s1 << game.getMovesList(i);
		s1 << " | ";
		OutputDebugStringA(s1.str().c_str());
		s1.str("");
		moves[i] = game.getMovesList(i);
	}
	data.clear();
	for (int i = 0; i < movesListCount; i++)
	{
		std::string move = moves[i];
		s1 << moves[i];
		OutputDebugStringA(s1.str().c_str());
		data.push_back(wxVariant(wxString(move)));
		if ((i+1) % 3 == 0)
		{
			movesList->AppendItem(data);
			data.clear();
		}
	}
	/*int moveNum = allMoves.size();
	s1 << moveNum;
	OutputDebugStringA(s1.str().c_str());
	for (int i = 0; i < moveCount; i++)
	{
		allMoves[i] = game.getMovesList(i);
	}*/
	/*for (auto& item : allMoves)
	{
	 	s1 << item.size();
		OutputDebugStringA(s1.str().c_str());
		movesList->AppendItem(item);
	}*/
	// for (int i = 0; i < game.nWidth; i++)
	// {
	// 	for (int j = 0; j < game.nHeight; j++)
	// 	{
	// 		s1 << nField[j * nFieldWidth + i];
	// 	}
	// }
	// OutputDebugStringA(s1.str().c_str());
	return true;
}

// TODO - figure out how to save a game and implement saving
bool cMain::save(wxString filename)
{
	for (int i = 0; i < game.nWidth; i++)
	{
		for (int j = 0; j < game.nHeight; j++)
		{
			char cell = nField[i * game.nWidth + j];
			// TODO - check if things like this should be changed. well, they probably should, could be fucked now after fixing the hack
			// TODO - maybe not. wait, maybe yes. x and y are swapped often in computer graphics and in this program. but i don't think
			// TODO - it matters because we are now handling them properly in the corresponding functions
			game.setnField(i, j, cell);
		}
	}

	game.setCurrentPlayer(currentPlayer);
	game.setRemainingDisks(remainingDisks);
	game.setMoveCount(moveCount);
	game.setReverseCounter(reverseCounter);
	game.setMovesListCount(movesListCount);
	s1 << movesListCount;
	OutputDebugStringA(s1.str().c_str());
	s1.str("");
	for (int i = 0; i < movesListCount; i++)
	{
		std::string move = moves[i];
		/*s1 << move;
		s1 << " ";
		OutputDebugStringA(s1.str().c_str());
		s1.str("");*/
		game.setMovesList(i, move);
	}

	return game.save(filename.wc_str());
}
