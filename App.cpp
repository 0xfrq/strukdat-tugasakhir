#include "App.h"
#include "MainFrame.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(App);

bool App::OnInit() {
	MainFrame* frame = new MainFrame("Assets Tracker");
	frame->SetClientSize(800, 600);
	frame->Center();
	frame->Show();
	return true;
}
