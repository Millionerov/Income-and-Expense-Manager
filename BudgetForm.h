#pragma once

#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>
#using <System.dll>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;

namespace ExpenseTrackerApp {
    public ref class BudgetForm {
    private:
        Form^ form;
        TextBox^ textBoxBudget;
        Button^ btnOK;
        Button^ btnCancel;
        Label^ labelBudget;
        Label^ labelInfo;

    public:
        BudgetForm(double currentBudget);
        ~BudgetForm();
        double GetBudget();
        DialogResult ShowDialog();
        void Close();
    };
}