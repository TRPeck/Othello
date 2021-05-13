#pragma once

#include "wx/wx.h"

class cSettings : public wxDialog
{
public:
	cSettings(const wxString &title, int configuration);
	~cSettings();

	wxRadioButton* rb = nullptr;
	wxRadioButton* rb1 = nullptr;

	wxRadioButton* rb2 = nullptr;
	wxRadioButton* rb3 = nullptr;

	wxButton* ok = nullptr;
	wxButton* close = nullptr;

	void okButtonClicked(wxCommandEvent& evt);
	void closeButtonClicked(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();
};

