#include "cMain.h"

#include "cApp.h"

// TODO - refactor to follow proper coding conventions
// REVIEW - check controls + their use for cross-platform compatibility

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_MENU(20001, cMain::onMenuNew)
	EVT_MENU(20002, cMain::onMenuSave)
	EVT_MENU(20003, cMain::onMenuOpen)
	EVT_MENU(20004, cMain::onMenuExit)
	EVT_MENU(20005, cMain::onMenuPSettings)
	EVT_GRID_CELL_LEFT_CLICK(cMain::getState)
	EVT_SIZE(cMain::windowResized)
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

	mainSizer = new wxBoxSizer(wxHORIZONTAL); // sizer to separate board and panel showing the moves/timer

	// NOTE - probably should have the grid inside a panel like this. wasn't working though, possibly due to the size.
	//wxPanel* gamePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(700, 600));
	infoPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(300, 600));
	infoPanel->SetBackgroundColour(*wxBLACK);
	auto infoPanelSizer = new wxBoxSizer(wxVERTICAL);

	movesList = new wxGrid(infoPanel, wxID_ANY, wxDefaultPosition, wxSize(275, 545));
	movesList->CreateGrid(0, 2);
	movesList->SetColLabelValue(0, wxString("Black"));
	movesList->SetColLabelValue(1, wxString("White"));
	movesList->EnableEditing(false);  // set all grid cells to read only so the user can't edit them
	movesList->SetCellHighlightColour(wxColour(173, 216, 230));
	movesList->SetCellHighlightPenWidth(3);
	movesList->Layout();

	infoPanelSizer->Add(movesList, 1, wxEXPAND);

	// panel and buttons below movesList to navigate through grid
	auto gridBtnPanel = new wxPanel(infoPanel, wxID_ANY, wxDefaultPosition, wxSize(275, 45));
	firstMoveBtn = new wxButton(gridBtnPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(75, 50));
	prevMoveBtn = new wxButton(gridBtnPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(75, 50));
	nextMoveBtn = new wxButton(gridBtnPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(75, 50));
	lastMoveBtn = new wxButton(gridBtnPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(75, 50));

	// NOTE - buttons won't expand with bitmaps. gonna have to do that wxImage thing i assume
	/*firstMoveBtn->SetBitmap(wxBitmap(*firstMoveImg), wxLEFT);
	prevMoveBtn->SetBitmap(*prevMoveImg, wxLEFT);
	nextMoveBtn->SetBitmap(*nextMoveImg, wxLEFT);
	lastMoveBtn->SetBitmap(*lastMoveImg, wxLEFT);*/
	
	// sizer for the horizontally laid out buttons
	auto gridBtnSizer = new wxBoxSizer(wxHORIZONTAL);
	gridBtnSizer->Add(firstMoveBtn, 1, wxEXPAND );
	gridBtnSizer->Add(prevMoveBtn, 1, wxEXPAND);
	gridBtnSizer->Add(nextMoveBtn, 1, wxEXPAND);
	gridBtnSizer->Add(lastMoveBtn, 1, wxEXPAND);
	gridBtnPanel->SetSizerAndFit(gridBtnSizer);

	// REVIEW - should maybe have a main vertical sizer, and put buttons/timers below board/movesList, further info below that
	// add buttons below infoPanel - expand horizontally but not vertically
	infoPanelSizer->Add(gridBtnPanel, 0, wxEXPAND);

	

	btn = new cBoardSquare*[nFieldWidth * nFieldHeight];
	// NOTE - with how this sizers elements and the window are sized, things get pretty stretched out when we maximize
	// holds our buttons and rank/file labels
	auto grid = new wxFlexGridSizer(nFieldWidth + 2, nFieldHeight + 2, 0, 0); // we add two to each for rank/file labels
	
	for (int i = 1; i < 9; i++)  // make the ranks/files growable so they will expand properly
	{
		grid->AddGrowableRow(i, 1);
		grid->AddGrowableCol(i, 1);
	}
	// represents whether the squares are empty or have certain pieces on them
	nField = new char[nFieldWidth * nFieldHeight];
	// strings representing the moves made, for use in saving/loading and movesList
	moves = new std::string[180];

	this->SetBackgroundColour(*wxBLACK);

	// convoluted nested for loops/multiple if statements for the exterior cells of our grid to create the border.
	for (int i = 0; i < nFieldWidth + 2; i++)
	{
		if (i == 0 || i == 9) // if in the first or last column
		{
			auto text = new wxStaticText(this, wxID_ANY, " ");  // blank for the top corners of the grid
			text->SetForegroundColour(*wxWHITE);  // sets the font of the text
			grid->Add(text, 0, wxEXPAND | wxALL, 2);
			for (int x = 1; x < 9; x++)  // sets the static text representing the file numbers
			{
				int n = x + 96;  
				char rank = n;
				auto text = new wxStaticText(this, wxID_ANY, rank);
				text->SetForegroundColour(*wxWHITE);
				grid->Add(text, 0, wxEXPAND | wxALL | wxALIGN_CENTRE_HORIZONTAL, 2);
			}
			auto text2 = new wxStaticText(this, wxID_ANY, " "); // corresponding blank entry for the bottom corners of the grid
			grid->Add(text2, 0, wxEXPAND | wxALL, 2);
			text->SetForegroundColour(*wxWHITE);
		}
		else
		{
			for (int j = 0; j < nFieldHeight + 2; j++)
			{
				if (j == 0 || j == 9) // if in the first or last row, set the rank letters
				{
					char file = i + 48;
					auto text = new wxStaticText(this, wxID_ANY, file);
					text->SetForegroundColour(*wxWHITE);
					grid->Add(text, 0,wxEXPAND | wxALL | wxALIGN_CENTRE_VERTICAL, 2);
				}
				else
				{
					// create buttons representing squares, set colour of the board, bind clicking functionality, and set field
					// that gives a char representation of the board state
					btn[(i - 1) * nFieldWidth + (j - 1)] = new cBoardSquare(this, 10000 + ((i-1) * nFieldWidth + (j-1)));
					btn[(i - 1) * nFieldWidth + (j - 1)]->SetBackgroundColour(wxColour(34, 139, 34));
					grid->Add(btn[(i - 1) * nFieldWidth + (j - 1)], 1, wxEXPAND | wxALL, 1);
					btn[(i - 1) * nFieldWidth + (j - 1)]->Bind(
						wxEVT_COMMAND_BUTTON_CLICKED, &cMain::squareClicked, this);
					nField[(i - 1) * nFieldWidth + (j - 1)] = ' ';
				}
			}
		}
	}
	// set initial four tiles in center
	btn[27]->setSquareStatus('W'); // indicates how the square will be drawn
	btn[28]->setSquareStatus('B');
	btn[35]->setSquareStatus('B');
	btn[36]->setSquareStatus('W');
	btn[27]->Enable(false); // disable the button so it can't be clicked after a disk is placed
	btn[28]->Enable(false);
	btn[35]->Enable(false);
	btn[36]->Enable(false);
	nField[27] = 'W';
	nField[28] = 'B';
	nField[35] = 'B';
	nField[36] = 'W';

	infoPanel->SetSizerAndFit(infoPanelSizer);
	mainSizer->Add(grid, 2, wxEXPAND | wxALL, 5);
	mainSizer->Add(infoPanel, 1, wxEXPAND | wxTOP | wxRIGHT | wxBOTTOM, 5);
	this->SetSizerAndFit(mainSizer);

	// NOTE - not sure setting this here does anything
	movesList->SetRowLabelSize(infoPanelSizer->GetSize().GetWidth() / 5);
	movesList->SetColSize(0, infoPanelSizer->GetSize().GetWidth() / 2.5);
	movesList->SetColSize(1, infoPanelSizer->GetSize().GetWidth() / 2.5);
	movesList->GetGridWindow()->Bind(wxEVT_MOTION, &cMain::rangeSelect, this); // bind function that prevents range select to grid

	movesList->AppendRows(1);
	movesList->SetRowLabelValue(rowCount, wxString(std::to_string(moveCount)));
}

cMain::~cMain()
{
	delete[]btn;
	delete[]moves;
	delete[]nField;
}

// getter and setter for the configuration variable that indicates whether the two players will be ai or human
int cMain::getConfiguration()
{
	return configuration;
}

void cMain::setConfiguration(int c)
{
	configuration = c;
}

std::vector<int> cMain::checkLeft(int x, int y)
{
	std::vector<int> flipList;
	if (y != 0)
	{
		y--; // get the next square in the direction of the move. we only need the succeeding squares, not the one that was clicked
		while (y >= 0) // while we haven't reached the edge of the board
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

// TODO - comment the peculiarities in these that are fairly distinct from the first one
std::vector<int> cMain::checkRight(int x, int y)
{
	std::vector<int> flipList;
	if (y != nFieldWidth - 1)
	{
		y++;
		while (y <= nFieldWidth - 1)
		{
			if (y != nFieldWidth - 1 && nField[x * nFieldWidth + y] != currentPlayer && nField[x * nFieldWidth + y] !=
				' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				y++;
			}
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
		x--;
		while (x >= 0)
		{
			if (x != 0 && nField[x * nFieldWidth + y] != currentPlayer && nField[x * nFieldWidth + y] != ' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				x--;
			}
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
			if (x != nFieldHeight - 1 && nField[x * nFieldWidth + y] != currentPlayer && nField[x * nFieldWidth + y] != ' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				x++;
			}
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
			if (x != 0 && y != 0 && nField[x * nFieldWidth + y] != currentPlayer && nField[x * nFieldWidth + y] != ' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				x--;
				y--;
			}
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
			if (x != 0 && y != nFieldWidth - 1 && nField[x * nFieldWidth + y] != currentPlayer && nField[x * nFieldWidth
				+ y] != ' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				x--;
				y++;
			}
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
			if (x != nFieldHeight - 1 && y != 0 && nField[x * nFieldWidth + y] != currentPlayer && nField[x *
				nFieldWidth + y] != ' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				x++;
				y--;
			}
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
			if (x != nFieldHeight - 1 && y != nFieldWidth - 1 && nField[x * nFieldWidth + y] != currentPlayer && nField[
				x * nFieldWidth + y] != ' ')
			{
				flipList.push_back(x * nFieldWidth + y);
				x++;
				y++;
			}
			else if (nField[x * nFieldWidth + y] == ' ')
			{
				flipList.clear();
				break;
			}
			else if ((x == nFieldHeight - 1 || y == nFieldWidth - 1) && nField[x * nFieldWidth + y] != currentPlayer)
			{
				flipList.clear();
				break;
			}
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

	// iterate through all the squares of the board
	for (int x = 0; x < nFieldWidth; x++)
	{
		for (int y = 0; y < nFieldHeight; y++)
		{
			if (nField[x * nFieldWidth + y] == ' ')
			{
				// check each direction from each square for potential moves
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
		// if we have legal moves, the player will be able to make one. copy the nfield to save the board before the move
		strncpy(states[movesListCount], nField, 64);
		return true;
	}
	// if we don't have a legal move, skip the turn and adjust variables and controls accordingly
	strncpy(states[movesListCount], nField, 64);
	if (currentPlayer == 'W')
	{
		movesList->SetCellValue(rowCount, 1, wxString("N/A"));
		moveCount++;
		movesList->AppendRows(1);
		rowCount++;
		movesList->SetRowLabelValue(rowCount, wxString(std::to_string(moveCount)));
		moves[movesListCount] = "N/A";
		movesListCount++;
	}
	else
	{
		moveCount++;
		movesList->SetCellValue(rowCount, 0, wxString("N/A"));
		moves[movesListCount] = "N/A";
		movesListCount++;
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

	if (!flipList.empty())  // if the square that was clicked was a legal move and therefore flips some of the opponent's disks
	{
		for (auto i = 0; i < flipList.size(); i++)
		{
			// set the field and buttons accordingly
			nField[flipList[i]] = currentPlayer;
			btn[flipList[i]]->setSquareStatus(currentPlayer);
			btn[flipList[i]]->Enable(false);
		}
		return true;
	}
	return false;
}

void cMain::makeMove(int x, int y)
{
	// REVIEW - probably best to move redundant code outside if statement
	if (currentPlayer == 'B')
	{
		nField[x * nFieldWidth + y] = 'B'; 
		btn[x * nFieldWidth + y]->setSquareStatus('B');  // squareStatus tells the square what colour disk to display
		btn[x * nFieldWidth + y]->paintNow();  // having changed the status, we now redraw the square
		remainingDisks--;
		movesList->SetCellValue(rowCount, 0, wxString(getPosition(x, y)));
		moves[movesListCount] = getPosition(x, y);  // store the move as a string, for repopulating our movesList grid when loading
		movesListCount++;
		moveCount++;  // we keep a different count of moves here starting at 1 for labeling purposes
		currentPlayer = 'W';
	}
	else
	{
		nField[x * nFieldWidth + y] = 'W';
		btn[x * nFieldWidth + y]->paintNow();
		btn[x * nFieldWidth + y]->setSquareStatus('W');
		remainingDisks--;
		movesList->SetCellValue(rowCount, 1, wxString(getPosition(x, y)));
		moveCount++;
		movesList->AppendRows(1);
		rowCount++;
		movesList->SetRowLabelValue(rowCount, wxString(std::to_string(moveCount)));
		moves[movesListCount] = getPosition(x, y);
		movesListCount++;
		currentPlayer = 'B';
	}
	btn[x * nFieldWidth + y]->Enable(false);
	btn[x * nFieldWidth + y]->GetParent()->Refresh();
}

void cMain::countDisks()  // count the disks for each player to get score at end of game
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

std::string cMain::getPosition(int x, int y)  // get the rank and file from the position in the field
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

		// REVIEW - should probably disable the buttons and some functionality at this point

		return true;
	}
	return false;
}

void cMain::squareClicked(wxCommandEvent& evt)
{
	// TODO - handle move made when in previous game state
	if (prevState)
	{
		
	}

	// check if the board is full or there are no legal move's for each player
	checkEndGame();

	// get the coordinates of that square that was clicked from our button's id
	int x = (evt.GetId() - 10000) / nFieldWidth;
	int y = (evt.GetId() - 10000) % nFieldWidth;

	// TODO - logic for each player1 + player2 configuration
	switch (configuration)
	{
	case 0:			// player/player
		if (legalMoves())  // REVIEW - should probably check this after the opposing player's turn so a square doesn't have to be clicked
		{
			// REVIEW - can probably just check nfield here
			if (btn[x * nFieldWidth + y]->getSquareStatus() == ' ')
			{
				if (checkMove(x, y))  // make sure a legal square was clicked
				{
					makeMove(x, y);
				}
			}

			// set the bools tracking whether each player has legal moves to true for the case where a move is opened up
			// by the other player's move. if this isn't the case, this code will be skipped anyway.
			// REVIEW - this can probably be handled in the legalMoves function
			if (!blackLegalMoves || !whiteLegalMoves)
			{
				blackLegalMoves = true;
				whiteLegalMoves = true;
			}
		}
		else
		{
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
		if (currentPlayer == 'B')
		{
			if (legalMoves())
			{
				if (btn[x * nFieldWidth + y]->getSquareStatus() == ' ')
				{
					if (checkMove(x, y))
					{
						makeMove(x, y);
					}
				}

				if (legalMoves())
				{
					weakAIMove();

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
				if (!blackLegalMoves || !whiteLegalMoves)
				{
					blackLegalMoves = true;
					whiteLegalMoves = true;
				}
			}
			else
			{
				wxMessageBox("No legal moves. Passing turn to other player.");
				currentPlayer = 'W';
				blackLegalMoves = false;

				if (legalMoves())
				{
					weakAIMove();

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
		if (currentPlayer == 'W')
		{
			if (legalMoves())
			{
				if (btn[x * nFieldWidth + y]->getSquareStatus() == ' ')
				{
					if (checkMove(x, y))
					{
						makeMove(x, y);
					}
				}

				if (legalMoves())
				{
					weakAIMove();

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
				if (!blackLegalMoves || !whiteLegalMoves)
				{
					blackLegalMoves = true;
					whiteLegalMoves = true;
				}
			}
			else
			{
				wxMessageBox("No legal moves. Passing turn to other player.");
				currentPlayer = 'B';
				whiteLegalMoves = false;

				if (legalMoves())
				{
					weakAIMove();

					if (!blackLegalMoves || !whiteLegalMoves)
					{
						blackLegalMoves = true;
						whiteLegalMoves = true;
					}
				}
				else
				{
					currentPlayer = 'W';
					blackLegalMoves = false;
				}
			}
		}
		break;
	}

	
	evt.Skip(false);
}

void cMain::getState(wxGridEvent& evt)
{	
	int x = evt.GetRow();
	int y = evt.GetCol();

	// NOTE - clicking anywhere on the grid's overall window highlights the first cell. should try and get rid of this
	// REVIEW - going back to a previous state and clicking a button in an ai game fucks things up
	// REVIEW - might be better to make this a cell selected event rather than left click for use with the buttons. or maybe not.

	// highlight the cell that was clicked
	movesList->SetGridCursor(wxGridCellCoords(x, y));

	if (!prevState) // if we are coming from the current move
	{
		strncpy(states[movesListCount], nField, 64);  // copy the position as of this move so we can come back to it
		latestState = movesListCount;  // get the number of the latest move
		prevState = true;
	}
	else if ((x * 2 + y) == latestState) // if we are back on the current move
	{
		prevState = false;
	}

	strncpy(nField, states[x * 2 + y], 64);  // copy the state of the board at the time of selected move into nfield

	for (int i = 0; i < 64; i++)  // iterate through nfield, set the buttons according to its characters
	{
		//nField[i] = states[x * rowCount + y][i];
		if (nField[i] == ' ')
		{
			btn[i]->setSquareStatus(' ');
			btn[i]->Enable(true);
		}
		else if (nField[i] == 'B')
		{
			btn[i]->setSquareStatus('B');
			btn[i]->Enable(false);
		}
		else if (nField[i] == 'W')
		{
			btn[i]->setSquareStatus('W');
			btn[i]->Enable(false);
		}
	}
	this->Refresh();
	evt.Skip(false);
}

void cMain::weakAIMove()
{
	std::vector<int> movesVector = getLegalMoves();

	srand(time(NULL));

	int x = 0;
	int y = 0;

	// get an index for a random move from the legal moves
	int num = rand() % movesVector.size();
	int move = movesVector[num];

	// convert the 1d index into 2d coordinates
	x = move / nFieldWidth;
	y = move % nFieldWidth;

	// we know it's a legal move, but checkmove does part of the move making
	checkMove(x, y);
	makeMove(x, y);
}

void cMain::onMenuNew(wxCommandEvent& evt)
{
	// REVIEW - this seems to cause problems and lead to crashes
	// Reset game
	currentPlayer = 'B';
	remainingDisks = 60;
	scoreCount[0] = 0;
	scoreCount[1] = 0;
	moveCount = 0;
	//reverseCounter = 0;
	movesListCount = 0;
	//movesList->DeleteAllItems();
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
	// create and open up a new dialog for the player settings
	settings = new cSettings(wxT("Player Settings"), configuration);
	settings->Show();
	evt.Skip(false);

	// TODO - test this
	if (configuration == 2)
	{
		if (currentPlayer == 'B')
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
	if (configuration == 3)
	{
		while (!checkEndGame())
		{
			if (legalMoves())
			{
				weakAIMove();

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

// TODO - configure saving and loading for the new objects we are using with our wxgrid system
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
	//reverseCounter = game.getReverseCounter();
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
	//data.clear();
	for (int i = 0; i < movesListCount; i++)
	{
		std::string move = moves[i];
		s1 << moves[i];
		OutputDebugStringA(s1.str().c_str());
		//data.push_back(wxVariant(wxString(move)));
		if ((i+1) % 3 == 0)
		{
			//movesList->AppendItem(data);
			//data.clear();
		}
	}
	return true;
}

bool cMain::save(wxString filename)
{
	for (int i = 0; i < game.nWidth; i++)
	{
		for (int j = 0; j < game.nHeight; j++)
		{
			char cell = nField[i * game.nWidth + j];
			game.setnField(i, j, cell);
		}
	}

	game.setCurrentPlayer(currentPlayer);
	game.setRemainingDisks(remainingDisks);
	game.setMoveCount(moveCount);
	//game.setReverseCounter(reverseCounter);
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

void cMain::rangeSelect(wxMouseEvent& evt)
{
	// this basically makes it do nothing when trying to select multiple cells of the moveslist grid
	evt.Skip(false);
}

void cMain::windowResized(wxSizeEvent& evt)
{
	// ui components are resized after a resize event which breaks maximize, so we wait until after this event to call our resize function
	CallAfter(&cMain::resize);	

	evt.Skip();
}

void cMain::resize()
{
	movesList->SetRowLabelSize(infoPanel->GetSize().GetWidth() / 5); // set the row label size to 20% of its parent window
	movesList->SetColSize(0, infoPanel->GetSize().GetWidth() / 2.5); // set the columns' size to 40% of the parent window 
	movesList->SetColSize(1, infoPanel->GetSize().GetWidth() / 2.5);

	// scale movesList button images, convert to bitmaps, and set to buttons for proper fit when resizing
	firstMoveBtn->SetBitmap(wxBitmap(firstMoveImg->Scale(infoPanel->GetSize().GetWidth() / 4, 50)));
	firstMoveBtn->SetBitmapCurrent(firstMoveHoverImg->Scale(infoPanel->GetSize().GetWidth() / 4, 50));
	firstMoveBtn->SetBitmapPressed(firstMoveClickedImg->Scale(infoPanel->GetSize().GetWidth() / 4, 50));
	
	prevMoveBtn->SetBitmap(wxBitmap(prevMoveImg->Scale(infoPanel->GetSize().GetWidth() / 4, 50)));
	prevMoveBtn->SetBitmapCurrent(prevMoveHoverImg->Scale(infoPanel->GetSize().GetWidth() / 4, 50));
	prevMoveBtn->SetBitmapPressed(prevMoveClickedImg->Scale(infoPanel->GetSize().GetWidth() / 4, 50));

	nextMoveBtn->SetBitmap(wxBitmap(nextMoveImg->Scale(infoPanel->GetSize().GetWidth() / 4, 50)));
	nextMoveBtn->SetBitmapCurrent(nextMoveHoverImg->Scale(infoPanel->GetSize().GetWidth() / 4, 50));
	nextMoveBtn->SetBitmapPressed(nextMoveClickedImg->Scale(infoPanel->GetSize().GetWidth() / 4, 50));

	lastMoveBtn->SetBitmap(wxBitmap(lastMoveImg->Scale(infoPanel->GetSize().GetWidth() / 4, 50)));
	lastMoveBtn->SetBitmapCurrent(lastMoveHoverImg->Scale(infoPanel->GetSize().GetWidth() / 4, 50));
	lastMoveBtn->SetBitmapPressed(lastMoveClickedImg->Scale(infoPanel->GetSize().GetWidth() / 4, 50));
}

// TODO - create appropriate variables to track moves, program functionality, bind to buttons
void cMain::firstMoveBtnClicked(wxCommandEvent& evt)
{
}

void cMain::prevMoveBtnClicked(wxCommandEvent& evt)
{
}

void cMain::nextMoveBtnClicked(wxCommandEvent& evt)
{
}

void cMain::lastMoveBtnClicked(wxCommandEvent& evt)
{
}
