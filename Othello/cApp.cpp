#include "cApp.h"

wxIMPLEMENT_APP(cApp);

cApp::cApp()
{
}

cApp::~cApp()
{
}

bool cApp::OnInit()
{
	wxInitAllImageHandlers(); // needed for properly drawing buttons
	m_frame1 = new cMain();
	m_frame1->SetDoubleBuffered(true); // this prevents needing to refresh the buttons which go wonky after pressing alt
	m_frame1->Show();

	return true;
}

// gets the frame for our saving and loading functions
wxFrame* cApp::getFrame() const
{
	return m_frame1;
}
