#include "BudgetForm.h"

namespace ExpenseTrackerApp {
    BudgetForm::BudgetForm(double currentBudget) {
        form = gcnew Form();
        form->Text = L"Установить бюджет";
        form->Width = 380;
        form->Height = 180;
        form->BackColor = Color::FromArgb(32, 32, 32);
        form->ForeColor = Color::White;
        form->StartPosition = FormStartPosition::CenterParent;
        form->FormBorderStyle = FormBorderStyle::FixedDialog;
        form->MaximizeBox = false;
        form->MinimizeBox = false;

        labelBudget = gcnew Label();
        labelBudget->Text = L"Месячный бюджет (₽):";
        labelBudget->Location = Point(15, 20);
        labelBudget->Size = System::Drawing::Size(150, 30);
        labelBudget->ForeColor = Color::White;

        textBoxBudget = gcnew TextBox();
        textBoxBudget->Location = Point(165, 20);
        textBoxBudget->Width = 180;
        textBoxBudget->Text = currentBudget.ToString();
        textBoxBudget->BackColor = Color::FromArgb(45, 45, 48);
        textBoxBudget->ForeColor = Color::White;
        textBoxBudget->BorderStyle = BorderStyle::FixedSingle;

        labelInfo = gcnew Label();
        labelInfo->Text = L"Установите бюджет для отслеживания месячных расходов";
        labelInfo->Location = Point(15, 55);
        labelInfo->Size = System::Drawing::Size(340, 25);
        labelInfo->ForeColor = Color::Gray;
        labelInfo->Font = gcnew Drawing::Font(L"Segoe UI", 8, FontStyle::Italic);

        btnOK = gcnew Button();
        btnOK->Text = L"OK";
        btnOK->Location = Point(90, 95);
        btnOK->Size = System::Drawing::Size(90, 32);
        btnOK->FlatStyle = FlatStyle::Flat;
        btnOK->BackColor = Color::FromArgb(70, 70, 75);
        btnOK->ForeColor = Color::White;
        btnOK->DialogResult = System::Windows::Forms::DialogResult::OK;

        btnCancel = gcnew Button();
        btnCancel->Text = L"Отмена";
        btnCancel->Location = Point(195, 95);
        btnCancel->Size = System::Drawing::Size(90, 32);
        btnCancel->FlatStyle = FlatStyle::Flat;
        btnCancel->BackColor = Color::FromArgb(70, 70, 75);
        btnCancel->ForeColor = Color::White;
        btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;

        form->Controls->Add(labelBudget);
        form->Controls->Add(textBoxBudget);
        form->Controls->Add(labelInfo);
        form->Controls->Add(btnOK);
        form->Controls->Add(btnCancel);

        form->AcceptButton = btnOK;
        form->CancelButton = btnCancel;
    }

    BudgetForm::~BudgetForm() {
        delete form;
    }

    DialogResult BudgetForm::ShowDialog() {
        return form->ShowDialog();
    }

    void BudgetForm::Close() {
        form->Close();
    }

    double BudgetForm::GetBudget() {
        try {
            return Double::Parse(textBoxBudget->Text);
        }
        catch (...) {
            return 0;
        }
    }
}
