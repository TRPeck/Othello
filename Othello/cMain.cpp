#include "cMain.h"

#include "cApp.h"

// TODO - comments mafucka!!!

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_MENU(20001, cMain::onMenuNew)
	EVT_MENU(20002, cMain::onMenuSave)
	EVT_MENU(20003, cMain::onMenuOpen)
	EVT_MENU(20004, cMain::onMenuExit)
wxEND_EVENT_TABLE()

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "Othello", wxPoint(30, 30), wxSize(1000, 800))
{
	// TODO - add menu bar, fit it into the frame, add some options to it
	
	menuBar = new wxMenuBar();
	this->SetMenuBar(menuBar);
	auto menuFile = new wxMenu();
	menuFile->Append(20001, "New");
	menuFile->Append(20002, "Save");
	menuFile->Append(20003, "Open");
	menuFile->Append(20004, "Exit");
	menuBar->Append(menuFile, "File");

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
					btn[(j - 1) * nFieldWidth + (i - 1)] = new cBoardSquare(this, 10000 + (j * nFieldWidth + i));

					if (j % 2 == 0)
					{
						btn[(j - 1) * nFieldWidth + (i - 1)]->SetBackgroundColour(colour2);
					}
					else
					{
						btn[(j - 1) * nFieldWidth + (i - 1)]->SetBackgroundColour(colour1);
					}

					grid->Add(btn[(j - 1) * nFieldWidth + (i - 1)], 1, wxEXPAND | wxALL);
					btn[(j - 1) * nFieldWidth + (i - 1)]->Bind(
						wxEVT_COMMAND_BUTTON_CLICKED, &cMain::squareClicked, this);
					nField[(j - 1) * nFieldWidth + (i - 1)] = ' ';
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
}

bool cMain::legalMoves(char c)
{
	std::vector<int> flipList;
	std::vector<int> sub;

	for (int y = 0; y < nFieldWidth; y++)
	{
		for (int x = 0; x < nFieldHeight; x++)
		{
			if (nField[y * nFieldWidth + x] == ' ')
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
			}
		}
	}

	if (!flipList.empty())
	{
		return true;
	}
	return false;
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
		for (auto i = 0; i < flipList.size(); i++)
		{
			nField[flipList[i]] = currentPlayer;
			btn[flipList[i]]->setSquareStatus(currentPlayer);
			btn[flipList[i]]->Enable(false);
		}
		return true;
	}
	return false;
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
			if (y != 0 && nField[y * nFieldWidth + x] != currentPlayer && nField[y * nFieldWidth + x] != ' ')
			{
				flipList.push_back(y * nFieldWidth + x);
				y--;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[y * nFieldWidth + x] == ' ')
			{
				flipList.clear();
				break;
			}
				// break and clear if reached the end of the board and its not the current player's disk 
			else if (y == 0 && nField[y * nFieldWidth + x] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[y * nFieldWidth + x] == currentPlayer)
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
			if (y != nFieldWidth - 1 && nField[y * nFieldWidth + x] != currentPlayer && nField[y * nFieldWidth + x] !=
				' ')
			{
				flipList.push_back(y * nFieldWidth + x);
				y++;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[y * nFieldWidth + x] == ' ')
			{
				flipList.clear();
				break;
			}
			else if (y == nFieldWidth - 1 && nField[y * nFieldWidth + x] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[y * nFieldWidth + x] == currentPlayer)
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
			if (x != 0 && nField[y * nFieldWidth + x] != currentPlayer && nField[y * nFieldWidth + x] != ' ')
			{
				flipList.push_back(y * nFieldWidth + x);
				x--;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[y * nFieldWidth + x] == ' ')
			{
				flipList.clear();
				break;
			}
			else if (x == 0 && nField[y * nFieldWidth + x] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[y * nFieldWidth + x] == currentPlayer)
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
		x++; // get the next square to the left of the move
		while (x <= nFieldHeight - 1) // while we haven't reached the left side of the board
		{
			// add coordinates if they contain the other player's pieces
			if (x != nFieldHeight - 1 && nField[y * nFieldWidth + x] != currentPlayer && nField[y * nFieldWidth + x] !=
				' ')
			{
				flipList.push_back(y * nFieldWidth + x);
				x++;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[y * nFieldWidth + x] == ' ')
			{
				flipList.clear();
				break;
			}
			else if (x == nFieldHeight - 1 && nField[y * nFieldWidth + x] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[y * nFieldWidth + x] == currentPlayer)
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
			if (x != 0 && y != 0 && nField[y * nFieldWidth + x] != currentPlayer && nField[y * nFieldWidth + x] != ' ')
			{
				flipList.push_back(y * nFieldWidth + x);
				x--;
				y--;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[y * nFieldWidth + x] == ' ')
			{
				flipList.clear();
				break;
			}
			else if (x == 0 || y == 0 && nField[y * nFieldWidth + x] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[y * nFieldWidth + x] == currentPlayer)
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
			if (x != 0 && y != nFieldWidth - 1 && nField[y * nFieldWidth + x] != currentPlayer && nField[y * nFieldWidth
				+ x] != ' ')
			{
				flipList.push_back(y * nFieldWidth + x);
				x--;
				y++;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[y * nFieldWidth + x] == ' ')
			{
				flipList.clear();
				break;
			}
			else if (x == 0 || y == nFieldWidth - 1 && nField[y * nFieldWidth + x] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[y * nFieldWidth + x] == currentPlayer)
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
			if (x != nFieldHeight - 1 && y != 0 && nField[y * nFieldWidth + x] != currentPlayer && nField[y *
				nFieldWidth + x] != ' ')
			{
				flipList.push_back(y * nFieldWidth + x);
				x++;
				y--;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[y * nFieldWidth + x] == ' ')
			{
				flipList.clear();
				break;
			}
			else if (x == nFieldHeight - 1 || y == 0 && nField[y * nFieldWidth + x] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[y * nFieldWidth + x] == currentPlayer)
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
			if (x != nFieldHeight - 1 && y != nFieldWidth - 1 && nField[y * nFieldWidth + x] != currentPlayer && nField[
				y * nFieldWidth + x] != ' ')
			{
				flipList.push_back(y * nFieldWidth + x);
				x++;
				y++;
			}
				// break and clear the sublist if an empty square is encountered
			else if (nField[y * nFieldWidth + x] == ' ')
			{
				flipList.clear();
				break;
			}
			else if (x == nFieldHeight - 1 || y == nFieldWidth - 1 && nField[y * nFieldWidth + x] != currentPlayer)
			{
				flipList.clear();
				break;
			}
				// break if one of the player's pieces is encountered but preserve the vector telling us which squares to flip
			else if (nField[y * nFieldWidth + x] == currentPlayer)
			{
				break;
			}
		}
	}
	return flipList;
}

void cMain::countDisks()
{
	for (int y = 0; y < nFieldWidth; y++)
	{
		for (int x = 0; x < nFieldHeight; x++)
		{
			if (nField[y * nFieldWidth + x] == 'B')
			{
				scoreCount[0]++;
			}
			else if (nField[y * nFieldWidth + x] == 'W')
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
	if (remainingDisks == 0 || (!legalMoves('B') && !legalMoves('W')))
	{
		countDisks();
		if (scoreCount[0] > scoreCount[1])
		{
			wxMessageBox("BLACK: " + std::to_string(scoreCount[0]) + " WHITE: " + std::to_string(scoreCount[0]) +
				" . BLACK WINS.");
		}
		else if (scoreCount[0] < scoreCount[1])
		{
			wxMessageBox("BLACK: " + std::to_string(scoreCount[0]) + " WHITE: " + std::to_string(scoreCount[0]) +
				" . WHITE WINS.");
		}
		else
		{
			wxMessageBox("BLACK: " + std::to_string(scoreCount[0]) + " WHITE: " + std::to_string(scoreCount[0]) +
				" . DRAW.");
		}
		// Reset game
		currentPlayer = 'B';
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
				nField[j * nFieldWidth + i] = ' ';
				btn[j * nFieldWidth + i]->setSquareStatus(' ');
				btn[j * nFieldWidth + i]->Refresh();
				btn[j * nFieldWidth + i]->Enable(true);
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
		return true;
	}
	return false;
}

void cMain::squareClicked(wxCommandEvent& evt)
{
	// check for legal move
	// set square status for current player's colour, disable
	// check surrounding square's for pieces to be flipped, change their square status and redraw
	// check endgame conditions - we'll check if there are 0 disks, rather than search the board for empty squares
	// if endgame, display results and reset
	// if not, switch the current player

	//checkEndGame();

	int x = (evt.GetId() - 10000) % nFieldWidth;
	int y = (evt.GetId() - 10000) / nFieldWidth;
	x--;
	y--;
	s1 << movesListCount;
	OutputDebugStringA(s1.str().c_str());
	s1.str("");

	if (legalMoves(currentPlayer))
	{
		if (btn[y * nFieldWidth + x]->getSquareStatus() == ' ')
		{
			if (checkMove(x, y))
			{
				if (currentPlayer == 'B')
				{
					nField[y * nFieldWidth + x] = 'B';
					btn[y * nFieldWidth + x]->paintNow();
					currentPlayer = 'W';
					btn[y * nFieldWidth + x]->setSquareStatus('B');
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
				else
				{
					nField[y * nFieldWidth + x] = 'W';
					btn[y * nFieldWidth + x]->paintNow();
					currentPlayer = 'B';
					btn[y * nFieldWidth + x]->setSquareStatus('W');
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
				btn[y * nFieldWidth + x]->Enable(false);
			}
		}

		btn[y * nFieldWidth + x]->GetParent()->Refresh();
	}
	else
	{
		// TODO - set moves list appropriately for skipped turns
		wxMessageBox("No legal moves. Passing turn to other player.");
		if (currentPlayer == 'B')
		{
			currentPlayer = 'W';
		}
		else
		{
			currentPlayer = 'B';
		}
	}
	evt.Skip();
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
			nField[j * nFieldWidth + i] = ' ';
			btn[j * nFieldWidth + i]->setSquareStatus(' ');
			btn[j * nFieldWidth + i]->Refresh();
			btn[j * nFieldWidth + i]->Enable(true);
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
	evt.Skip();
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
	evt.Skip();
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
	evt.Skip();
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
			btn[j * nFieldWidth + i]->setSquareStatus(cell);
			nField[j * nFieldWidth + i] = cell;
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
			char cell = nField[j * game.nWidth + i];
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
