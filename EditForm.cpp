#include "EditForm.h"

namespace ExpenseTrackerApp {
    void EditForm::InitializeComponent() {
        this->SuspendLayout();

        this->BackColor = Color::FromArgb(32, 32, 32);
        this->ForeColor = Color::White;

        this->labelDesc = gcnew Label();
        this->labelDesc->Text = L"Описание:";
        this->labelDesc->Location = Point(15, 20);
        this->labelDesc->Size = System::Drawing::Size(90, 25);
        this->labelDesc->ForeColor = Color::White;

        this->textBoxEditDescription = gcnew TextBox();
        this->textBoxEditDescription->Location = Point(115, 20);
        this->textBoxEditDescription->Width = 220;
        this->textBoxEditDescription->BackColor = Color::FromArgb(45, 45, 48);
        this->textBoxEditDescription->ForeColor = Color::White;
        this->textBoxEditDescription->BorderStyle = BorderStyle::FixedSingle;

        this->labelAmount = gcnew Label();
        this->labelAmount->Text = L"Сумма:";
        this->labelAmount->Location = Point(15, 60);
        this->labelAmount->Size = System::Drawing::Size(90, 25);
        this->labelAmount->ForeColor = Color::White;

        this->textBoxEditAmount = gcnew TextBox();
        this->textBoxEditAmount->Location = Point(115, 60);
        this->textBoxEditAmount->Width = 150;
        this->textBoxEditAmount->BackColor = Color::FromArgb(45, 45, 48);
        this->textBoxEditAmount->ForeColor = Color::White;
        this->textBoxEditAmount->BorderStyle = BorderStyle::FixedSingle;

        this->labelCat = gcnew Label();
        this->labelCat->Text = L"Категория:";
        this->labelCat->Location = Point(15, 100);
        this->labelCat->Size = System::Drawing::Size(90, 25);
        this->labelCat->ForeColor = Color::White;

        this->comboBoxEditCategory = gcnew ComboBox();
        this->comboBoxEditCategory->Location = Point(115, 100);
        this->comboBoxEditCategory->Width = 160;
        this->comboBoxEditCategory->DropDownStyle = ComboBoxStyle::DropDownList;
        this->comboBoxEditCategory->BackColor = Color::FromArgb(45, 45, 48);
        this->comboBoxEditCategory->ForeColor = Color::White;
        this->comboBoxEditCategory->FlatStyle = FlatStyle::Flat;
        this->comboBoxEditCategory->Items->Add(L"Еда");
        this->comboBoxEditCategory->Items->Add(L"Транспорт");
        this->comboBoxEditCategory->Items->Add(L"Развлечения");
        this->comboBoxEditCategory->Items->Add(L"Здоровье");
        this->comboBoxEditCategory->Items->Add(L"Покупки");
        this->comboBoxEditCategory->Items->Add(L"Другое");
        this->comboBoxEditCategory->SelectedIndex = 0;

        this->labelType = gcnew Label();
        this->labelType->Text = L"Тип:";
        this->labelType->Location = Point(15, 140);
        this->labelType->Size = System::Drawing::Size(90, 25);
        this->labelType->ForeColor = Color::White;

        this->comboBoxEditType = gcnew ComboBox();
        this->comboBoxEditType->Location = Point(115, 140);
        this->comboBoxEditType->Width = 160;
        this->comboBoxEditType->DropDownStyle = ComboBoxStyle::DropDownList;
        this->comboBoxEditType->BackColor = Color::FromArgb(45, 45, 48);
        this->comboBoxEditType->ForeColor = Color::White;
        this->comboBoxEditType->FlatStyle = FlatStyle::Flat;
        this->comboBoxEditType->Items->Add(L"Расход");
        this->comboBoxEditType->Items->Add(L"Доход");
        this->comboBoxEditType->SelectedIndex = 0;

        this->labelHint = gcnew Label();
        this->labelHint->Text = L"Двойной клик для редактирования записи";
        this->labelHint->Location = Point(15, 180);
        this->labelHint->Size = System::Drawing::Size(250, 20);
        this->labelHint->ForeColor = Color::Gray;
        this->labelHint->Font = gcnew Drawing::Font(L"Segoe UI", 8, FontStyle::Italic);

        this->btnEditOK = gcnew Button();
        this->btnEditOK->Text = L"Сохранить";
        this->btnEditOK->Location = Point(60, 215);
        this->btnEditOK->Size = System::Drawing::Size(100, 35);
        this->btnEditOK->FlatStyle = FlatStyle::Flat;
        this->btnEditOK->BackColor = Color::FromArgb(70, 70, 75);
        this->btnEditOK->ForeColor = Color::White;
        this->btnEditOK->DialogResult = System::Windows::Forms::DialogResult::OK;
        this->btnEditOK->Click += gcnew EventHandler(this, &EditForm::btnEditOK_Click);

        this->btnEditCancel = gcnew Button();
        this->btnEditCancel->Text = L"Отмена";
        this->btnEditCancel->Location = Point(175, 215);
        this->btnEditCancel->Size = System::Drawing::Size(100, 35);
        this->btnEditCancel->FlatStyle = FlatStyle::Flat;
        this->btnEditCancel->BackColor = Color::FromArgb(70, 70, 75);
        this->btnEditCancel->ForeColor = Color::White;
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
        this->Controls->Add(this->labelHint);
        this->Controls->Add(this->btnEditOK);
        this->Controls->Add(this->btnEditCancel);

        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
        this->Text = L"Редактировать запись";
        this->StartPosition = FormStartPosition::CenterParent;
        this->MaximizeBox = false;
        this->MinimizeBox = false;
        this->AcceptButton = this->btnEditOK;
        this->CancelButton = this->btnEditCancel;
        this->ClientSize = System::Drawing::Size(360, 275);

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
                MessageBox::Show(L"Сумма должна быть больше 0!", L"Ошибка",
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
