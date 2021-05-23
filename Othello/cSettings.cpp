#include "cSettings.h"

#include "cApp.h"

wxBEGIN_EVENT_TABLE(cSettings, wxDialog)
	EVT_BUTTON(10007, cSettings::okButtonClicked)
	EVT_BUTTON(10008, cSettings::closeButtonClicked)
wxEND_EVENT_TABLE()

cSettings::cSettings(const wxString& title, int configuration) : wxDialog(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(250, 500))
{
	wxPanel* panel = new wxPanel(this, wxID_ANY);
	wxBoxSizer* vert = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *hori = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBox* box = new wxStaticBox(panel, 10001, wxT("Black"), wxPoint(5, 5), wxSize(240, 150));
	wxStaticBox* box1 = new wxStaticBox(panel, 10002, wxT("White"), wxPoint(5, 255), wxSize(240, 150));

	// button group which sets the player for player one
	rb = new wxRadioButton(panel, 10003, wxT("Human"), wxPoint(15, 30), wxDefaultSize, wxRB_GROUP);
	rb1 = new wxRadioButton(panel, 10004, wxT("Weak AI"), wxPoint(15, 55));

	// button group which sets the player for player two
	rb2 = new wxRadioButton(panel, 10005, wxT("Human"), wxPoint(15, 280), wxDefaultSize, wxRB_GROUP);
	rb3 = new wxRadioButton(panel, 10006, wxT("Weak AI"), wxPoint(15, 305));

	ok = new wxButton(this, 10007, "OK", wxDefaultPosition, wxSize(70, 30));
	close = new wxButton(this, 10008, "Close", wxDefaultPosition, wxSize(70, 30));

	hori->Add(ok, 1);
	hori->Add(close, 1, wxLEFT, 5);

	vert->Add(panel, 1);
	vert->Add(hori, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

	SetSizer(vert);

	Centre();

	/* 0. human checked human checked
	   1. human checked weak ai checked
	   2. weak ai checked human checked
	   3. weak ai checked weak ai checked */

	switch(configuration) // sets which radio buttons are checked based on the configuration variable from cMain
	{
	case 0:
		rb->SetValue(true);
		rb2->SetValue(true);
		break;
	case 1:
		rb->SetValue(true);
		rb3->SetValue(true);
		break;
	case 2:
		rb1->SetValue(true);
		rb2->SetValue(true);
		break;
	case 3:
		rb1->SetValue(true);
		rb3->SetValue(true);
		break;
	}
	
	ShowModal();

	Destroy();
}

cSettings::~cSettings()
{
}

// set the configuration variable in cMain based on selected radio buttons
void cSettings::okButtonClicked(wxCommandEvent& evt)
{
	if (rb->GetValue() && rb2->GetValue()) // if human clicked and human clicked
	{
		static_cast<cMain*>(wxGetApp().getFrame())->setConfiguration(0);
	}
	else if (rb->GetValue() && rb3->GetValue()) // if human clicked and weak ai clicked
	{
		static_cast<cMain*>(wxGetApp().getFrame())->setConfiguration(1);
	}
	else if (rb1->GetValue() && rb2->GetValue()) // if weak ai clicked and human clicked
	{
		static_cast<cMain*>(wxGetApp().getFrame())->setConfiguration(2);
	}
	else if (rb1->GetValue() && rb3->GetValue()) // if weak ai clicked and weak i clicked
	{
		static_cast<cMain*>(wxGetApp().getFrame())->setConfiguration(3);
	}
	
	this->Destroy();
	evt.Skip(false);
}

void cSettings::closeButtonClicked(wxCommandEvent& evt)
{
	this->Destroy();
	evt.Skip(false);
}
