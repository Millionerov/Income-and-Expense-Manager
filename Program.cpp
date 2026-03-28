#include "Form1.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace ExpenseTrackerApp;

[STAThread]
int main() {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew Form1());
    return 0;
}
