#include "CalibrationUI.h"

using namespace System;
using namespace System::Windows::Forms;


[STAThread]
void Main(array<String^>^ args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	CalibrationApp::CalibrationUI form;
	Application::Run(%form);
}
