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
	wxInitAllImageHandlers();
	m_frame1 = new cMain();
	m_frame1->Show();

	return true;
}

wxFrame* cApp::getFrame() const
{
	return m_frame1;
}
