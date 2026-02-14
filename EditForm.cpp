#include "EditForm.h"

namespace ExpenseTrackerApp {
    void EditForm::InitializeComponent() {
        this->SuspendLayout();

        this->labelDesc = gcnew Label();
        this->labelDesc->Text = L"Описание:";
        this->labelDesc->Location = Point(10, 20);
        this->labelDesc->Width = 80;

        this->textBoxEditDescription = gcnew TextBox();
        this->textBoxEditDescription->Location = Point(100, 20);
        this->textBoxEditDescription->Width = 200;

        this->labelAmount = gcnew Label();
        this->labelAmount->Text = L"Сумма:";
        this->labelAmount->Location = Point(10, 60);
        this->labelAmount->Width = 80;

        this->textBoxEditAmount = gcnew TextBox();
        this->textBoxEditAmount->Location = Point(100, 60);
        this->textBoxEditAmount->Width = 100;

        this->labelCat = gcnew Label();
        this->labelCat->Text = L"Категория:";
        this->labelCat->Location = Point(10, 100);
        this->labelCat->Width = 80;

        this->comboBoxEditCategory = gcnew ComboBox();
        this->comboBoxEditCategory->Location = Point(100, 100);
        this->comboBoxEditCategory->Width = 120;
        this->comboBoxEditCategory->DropDownStyle = ComboBoxStyle::DropDownList;
        this->comboBoxEditCategory->Items->Add(L"Еда");
        this->comboBoxEditCategory->Items->Add(L"Транспорт");
        this->comboBoxEditCategory->Items->Add(L"Развлечения");
        this->comboBoxEditCategory->Items->Add(L"Здоровье");
        this->comboBoxEditCategory->Items->Add(L"Покупки");
        this->comboBoxEditCategory->Items->Add(L"Другое");
        this->comboBoxEditCategory->SelectedIndex = 0;

        this->labelType = gcnew Label();
        this->labelType->Text = L"Тип:";
        this->labelType->Location = Point(10, 140);
        this->labelType->Width = 80;

        this->comboBoxEditType = gcnew ComboBox();
        this->comboBoxEditType->Location = Point(100, 140);
        this->comboBoxEditType->Width = 120;
        this->comboBoxEditType->DropDownStyle = ComboBoxStyle::DropDownList;
        this->comboBoxEditType->Items->Add(L"Расход");
        this->comboBoxEditType->Items->Add(L"Доход");
        this->comboBoxEditType->SelectedIndex = 0;

        this->btnEditOK = gcnew Button();
        this->btnEditOK->Text = L"OK";
        this->btnEditOK->Location = Point(50, 180);
        this->btnEditOK->Width = 80;
        this->btnEditOK->DialogResult = System::Windows::Forms::DialogResult::OK;
        this->btnEditOK->Click += gcnew EventHandler(this, &EditForm::btnEditOK_Click);

        this->btnEditCancel = gcnew Button();
        this->btnEditCancel->Text = L"Отмена";
        this->btnEditCancel->Location = Point(150, 180);
        this->btnEditCancel->Width = 80;
        this->btnEditCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
        this->btnEditCancel->Click += gcnew EventHandler(this, &EditForm::btnEditCancel_Click);

        this->Controls->Add(this->labelDesc);
        this->Controls->Add(this->textBoxEditDescription);
        this->Controls->Add(this->labelAmount);
        this->Controls->Add(this->textBoxEditAmount);
        this->Controls->Add(this->labelCat);
        this->Controls->Add(this->comboBoxEditCategory);
        this->Controls->Add(this->labelType);
        this->Controls->Add(this->comboBoxEditType);
        this->Controls->Add(this->btnEditOK);
        this->Controls->Add(this->btnEditCancel);

        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
        this->Text = L"Редактировать запись";
        this->StartPosition = FormStartPosition::CenterParent;
        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
        this->MaximizeBox = false;
        this->MinimizeBox = false;
        this->AcceptButton = this->btnEditOK;
        this->CancelButton = this->btnEditCancel;

        this->ResumeLayout(false);
        this->PerformLayout();
    }

    EditForm::EditForm() {
        InitializeComponent();
    }

    void EditForm::btnEditOK_Click(Object^ sender, EventArgs^ e) {
        try {
            if (String::IsNullOrWhiteSpace(textBoxEditDescription->Text) ||
                String::IsNullOrWhiteSpace(textBoxEditAmount->Text)) {
                MessageBox::Show(L"Заполните все поля!", L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Warning);
                this->DialogResult = System::Windows::Forms::DialogResult::None;
                return;
            }

            double amt = Double::Parse(textBoxEditAmount->Text);
            if (amt <= 0) {
                MessageBox::Show(L"Сумма должна быть > 0!", L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Warning);
                this->DialogResult = System::Windows::Forms::DialogResult::None;
                return;
            }
        }
        catch (FormatException^) {
            MessageBox::Show(L"Введите корректную сумму!", L"Ошибка",
                MessageBoxButtons::OK, MessageBoxIcon::Error);
            this->DialogResult = System::Windows::Forms::DialogResult::None;
            return;
        }
    }

    void EditForm::btnEditCancel_Click(Object^ sender, EventArgs^ e) {
        this->Close();
    }

    void EditForm::SetData(String^ desc, String^ amount, Category cat, TransactionType type) {
        this->textBoxEditDescription->Text = desc;
        this->textBoxEditAmount->Text = amount;
        this->comboBoxEditCategory->SelectedIndex = (int)cat;
        this->comboBoxEditType->SelectedIndex = (int)type;
    }
}