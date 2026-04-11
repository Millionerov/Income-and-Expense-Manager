#pragma once

#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>

#include "ExpenseManager.h"
#include <msclr/marshal_cppstd.h>
using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;

namespace ExpenseTrackerApp {
    public ref class ReportForm {
    private:
        Form^ form;
        ExpenseManager* manager;
        ListView^ listViewReport;
        Button^ btnClose;
        Label^ labelTitle;

        void LoadReport();
        void btnClose_Click(Object^ sender, EventArgs^ e);

    public:
        ReportForm(ExpenseManager* mgr);
        ~ReportForm();
        DialogResult ShowDialog();
        void Close();
    };
}