#include "Form1.h"

namespace ExpenseTrackerApp {
    ref class ListViewItemComparer : public Collections::IComparer {
    private:
        int col;
        bool ascending;
    public:
        ListViewItemComparer(int column, bool asc) {
            col = column;
            ascending = asc;
        }

        virtual int Compare(Object^ x, Object^ y) {
            ListViewItem^ itemX = (ListViewItem^)x;
            ListViewItem^ itemY = (ListViewItem^)y;

            String^ textX = itemX->SubItems[col]->Text;
            String^ textY = itemY->SubItems[col]->Text;

            if (col == 0) {
                int idX = Convert::ToInt32(textX);
                int idY = Convert::ToInt32(textY);
                return ascending ? idX.CompareTo(idY) : idY.CompareTo(idX);
            }
            else if (col == 2) {
                double amountX = Convert::ToDouble(textX->Replace(L"₽", L"")->Replace(L"руб.", L"")->Trim());
                double amountY = Convert::ToDouble(textY->Replace(L"₽", L"")->Replace(L"руб.", L"")->Trim());
                return ascending ? amountX.CompareTo(amountY) : amountY.CompareTo(amountX);
            }
            else if (col == 5) {
                try {
                    DateTime dateX = DateTime::Parse(textX);
                    DateTime dateY = DateTime::Parse(textY);
                    return ascending ? dateX.CompareTo(dateY) : dateY.CompareTo(dateX);
                }
                catch (...) {
                    return ascending ?
                        String::Compare(textX, textY) :
                        String::Compare(textY, textX);
                }
            }
            else {
                return ascending ?
                    String::Compare(textX, textY) :
                    String::Compare(textY, textX);
            }
        }
    };

    Form1::Form1() {
        components = gcnew System::ComponentModel::Container();
        manager = new ExpenseManager();
        selectedTransactionId = -1;
        currentFilterCategory = L"Все категории";
        currentFilterType = L"Все типы";
        lastSortedColumn = -1;
        sortAscending = false;
        darkThemeEnabled = true;
        InitializeComponent();
        ApplyTheme(true);
        RefreshTransactionList();
        UpdateTotals();
    }

    Form1::~Form1() {
        if (components) delete components;
        if (manager) delete manager;
    }

    void Form1::InitializeComponent() {
        this->SuspendLayout();

        this->textBoxDescription = gcnew TextBox();
        this->textBoxAmount = gcnew TextBox();
        this->comboBoxCategory = gcnew ComboBox();
        this->comboBoxType = gcnew ComboBox();
        this->comboBoxFilter = gcnew ComboBox();
        this->comboBoxFilterType = gcnew ComboBox();
        this->dateTimePickerExpense = gcnew DateTimePicker();
        this->btnAdd = gcnew Button();
        this->btnUpdate = gcnew Button();
        this->btnDelete = gcnew Button();
        this->btnFilter = gcnew Button();
        this->btnTheme = gcnew Button();
        this->listViewExpenses = gcnew ListView();
        this->labelTotal = gcnew Label();
        this->labelIncome = gcnew Label();
        this->labelExpense = gcnew Label();
        this->labelBalance = gcnew Label();
        this->labelStats = gcnew Label();
        this->statusStrip = gcnew StatusStrip();
        this->statusLabel = gcnew ToolStripStatusLabel();

        Label^ labelDesc = gcnew Label();
        labelDesc->Text = L"Описание:";
        labelDesc->Location = Point(12, 12);
        labelDesc->Size = System::Drawing::Size(70, 25);
        labelDesc->TextAlign = ContentAlignment::MiddleRight;

        Label^ labelAmount = gcnew Label();
        labelAmount->Text = L"Сумма:";
        labelAmount->Location = Point(12, 42);
        labelAmount->Size = System::Drawing::Size(70, 25);
        labelAmount->TextAlign = ContentAlignment::MiddleRight;

        Label^ labelCat = gcnew Label();
        labelCat->Text = L"Категория:";
        labelCat->Location = Point(12, 72);
        labelCat->Size = System::Drawing::Size(70, 25);
        labelCat->TextAlign = ContentAlignment::MiddleRight;

        Label^ labelType = gcnew Label();
        labelType->Text = L"Тип:";
        labelType->Location = Point(12, 102);
        labelType->Size = System::Drawing::Size(70, 25);
        labelType->TextAlign = ContentAlignment::MiddleRight;

        Label^ labelDate = gcnew Label();
        labelDate->Text = L"Дата:";
        labelDate->Location = Point(12, 132);
        labelDate->Size = System::Drawing::Size(70, 25);
        labelDate->TextAlign = ContentAlignment::MiddleRight;

        Label^ labelFilter = gcnew Label();
        labelFilter->Text = L"Категория:";
        labelFilter->Location = Point(320, 12);
        labelFilter->Size = System::Drawing::Size(70, 25);
        labelFilter->TextAlign = ContentAlignment::MiddleRight;

        Label^ labelFilterType = gcnew Label();
        labelFilterType->Text = L"Тип:";
        labelFilterType->Location = Point(320, 42);
        labelFilterType->Size = System::Drawing::Size(70, 25);
        labelFilterType->TextAlign = ContentAlignment::MiddleRight;

        this->textBoxDescription->Location = Point(90, 12);
        this->textBoxDescription->Width = 200;
        this->textBoxDescription->BorderStyle = BorderStyle::FixedSingle;

        this->textBoxAmount->Location = Point(90, 42);
        this->textBoxAmount->Width = 120;
        this->textBoxAmount->BorderStyle = BorderStyle::FixedSingle;

        this->comboBoxCategory->Location = Point(90, 72);
        this->comboBoxCategory->Width = 140;
        this->comboBoxCategory->DropDownStyle = ComboBoxStyle::DropDownList;
        this->comboBoxCategory->FlatStyle = FlatStyle::Flat;
        this->comboBoxCategory->Items->Add(L"Еда");
        this->comboBoxCategory->Items->Add(L"Транспорт");
        this->comboBoxCategory->Items->Add(L"Развлечения");
        this->comboBoxCategory->Items->Add(L"Здоровье");
        this->comboBoxCategory->Items->Add(L"Покупки");
        this->comboBoxCategory->Items->Add(L"Другое");
        this->comboBoxCategory->SelectedIndex = 0;

        this->comboBoxType->Location = Point(90, 102);
        this->comboBoxType->Width = 140;
        this->comboBoxType->DropDownStyle = ComboBoxStyle::DropDownList;
        this->comboBoxType->FlatStyle = FlatStyle::Flat;
        this->comboBoxType->Items->Add(L"Расход");
        this->comboBoxType->Items->Add(L"Доход");
        this->comboBoxType->SelectedIndex = 0;

        this->dateTimePickerExpense->Location = Point(90, 132);
        this->dateTimePickerExpense->Width = 140;
        this->dateTimePickerExpense->Format = DateTimePickerFormat::Short;

        this->comboBoxFilter->Location = Point(400, 12);
        this->comboBoxFilter->Width = 150;
        this->comboBoxFilter->DropDownStyle = ComboBoxStyle::DropDownList;
        this->comboBoxFilter->FlatStyle = FlatStyle::Flat;
        this->comboBoxFilter->Items->Add(L"Все категории");
        this->comboBoxFilter->Items->Add(L"Еда");
        this->comboBoxFilter->Items->Add(L"Транспорт");
        this->comboBoxFilter->Items->Add(L"Развлечения");
        this->comboBoxFilter->Items->Add(L"Здоровье");
        this->comboBoxFilter->Items->Add(L"Покупки");
        this->comboBoxFilter->Items->Add(L"Другое");
        this->comboBoxFilter->SelectedIndex = 0;
        this->comboBoxFilter->SelectedIndexChanged += gcnew EventHandler(this, &Form1::comboBoxFilter_SelectedIndexChanged);

        this->comboBoxFilterType->Location = Point(400, 42);
        this->comboBoxFilterType->Width = 150;
        this->comboBoxFilterType->DropDownStyle = ComboBoxStyle::DropDownList;
        this->comboBoxFilterType->FlatStyle = FlatStyle::Flat;
        this->comboBoxFilterType->Items->Add(L"Все типы");
        this->comboBoxFilterType->Items->Add(L"Расход");
        this->comboBoxFilterType->Items->Add(L"Доход");
        this->comboBoxFilterType->SelectedIndex = 0;
        this->comboBoxFilterType->SelectedIndexChanged += gcnew EventHandler(this, &Form1::comboBoxFilterType_SelectedIndexChanged);

        this->btnAdd->Text = L"Добавить";
        this->btnAdd->Location = Point(90, 170);
        this->btnAdd->Size = System::Drawing::Size(100, 32);
        this->btnAdd->FlatStyle = FlatStyle::Flat;
        this->btnAdd->Click += gcnew EventHandler(this, &Form1::btnAdd_Click);

        this->btnUpdate->Text = L"Обновить";
        this->btnUpdate->Location = Point(200, 170);
        this->btnUpdate->Size = System::Drawing::Size(100, 32);
        this->btnUpdate->FlatStyle = FlatStyle::Flat;
        this->btnUpdate->Click += gcnew EventHandler(this, &Form1::btnUpdate_Click);

        this->btnDelete->Text = L"Удалить";
        this->btnDelete->Location = Point(310, 170);
        this->btnDelete->Size = System::Drawing::Size(100, 32);
        this->btnDelete->FlatStyle = FlatStyle::Flat;
        this->btnDelete->Click += gcnew EventHandler(this, &Form1::btnDelete_Click);

        this->btnFilter->Text = L"Сбросить фильтр";
        this->btnFilter->Location = Point(560, 10);
        this->btnFilter->Size = System::Drawing::Size(110, 28);
        this->btnFilter->FlatStyle = FlatStyle::Flat;
        this->btnFilter->Click += gcnew EventHandler(this, &Form1::btnFilter_Click);

        this->btnTheme->Text = L"Светлая тема";
        this->btnTheme->Location = Point(560, 42);
        this->btnTheme->Size = System::Drawing::Size(110, 28);
        this->btnTheme->FlatStyle = FlatStyle::Flat;
        this->btnTheme->Click += gcnew EventHandler(this, &Form1::btnTheme_Click);

        this->listViewExpenses->Location = Point(12, 210);
        this->listViewExpenses->Size = System::Drawing::Size(660, 250);
        this->listViewExpenses->View = View::Details;
        this->listViewExpenses->FullRowSelect = true;
        this->listViewExpenses->GridLines = true;
        this->listViewExpenses->MultiSelect = false;
        this->listViewExpenses->AllowColumnReorder = true;
        this->listViewExpenses->SelectedIndexChanged += gcnew EventHandler(this, &Form1::listViewExpenses_SelectedIndexChanged);
        this->listViewExpenses->ColumnClick += gcnew ColumnClickEventHandler(this, &Form1::listViewExpenses_ColumnClick);
        this->listViewExpenses->MouseDoubleClick += gcnew MouseEventHandler(this, &Form1::listViewExpenses_MouseDoubleClick);
        this->listViewExpenses->Columns->Add(L"ID", 40, HorizontalAlignment::Left);
        this->listViewExpenses->Columns->Add(L"Описание", 180, HorizontalAlignment::Left);
        this->listViewExpenses->Columns->Add(L"Сумма", 100, HorizontalAlignment::Right);
        this->listViewExpenses->Columns->Add(L"Категория", 100, HorizontalAlignment::Left);
        this->listViewExpenses->Columns->Add(L"Тип", 80, HorizontalAlignment::Left);
        this->listViewExpenses->Columns->Add(L"Дата", 120, HorizontalAlignment::Left);

        this->labelTotal->Location = Point(12, 470);
        this->labelTotal->Size = System::Drawing::Size(200, 25);
        this->labelTotal->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);

        this->labelIncome->Location = Point(220, 470);
        this->labelIncome->Size = System::Drawing::Size(150, 25);

        this->labelExpense->Location = Point(380, 470);
        this->labelExpense->Size = System::Drawing::Size(150, 25);

        this->labelBalance->Location = Point(540, 470);
        this->labelBalance->Size = System::Drawing::Size(150, 25);
        this->labelBalance->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);

        this->labelStats->Location = Point(12, 500);
        this->labelStats->Size = System::Drawing::Size(660, 40);
        this->labelStats->Font = gcnew System::Drawing::Font(L"Segoe UI", 8);

        this->statusStrip->Items->Add(this->statusLabel);
        this->statusStrip->Location = Point(0, 545);
        this->statusStrip->Size = System::Drawing::Size(684, 22);

        this->Controls->Add(labelDesc);
        this->Controls->Add(this->textBoxDescription);
        this->Controls->Add(labelAmount);
        this->Controls->Add(this->textBoxAmount);
        this->Controls->Add(labelCat);
        this->Controls->Add(this->comboBoxCategory);
        this->Controls->Add(labelType);
        this->Controls->Add(this->comboBoxType);
        this->Controls->Add(labelDate);
        this->Controls->Add(this->dateTimePickerExpense);
        this->Controls->Add(labelFilter);
        this->Controls->Add(this->comboBoxFilter);
        this->Controls->Add(labelFilterType);
        this->Controls->Add(this->comboBoxFilterType);
        this->Controls->Add(this->btnAdd);
        this->Controls->Add(this->btnUpdate);
        this->Controls->Add(this->btnDelete);
        this->Controls->Add(this->btnFilter);
        this->Controls->Add(this->btnTheme);
        this->Controls->Add(this->listViewExpenses);
        this->Controls->Add(this->labelTotal);
        this->Controls->Add(this->labelIncome);
        this->Controls->Add(this->labelExpense);
        this->Controls->Add(this->labelBalance);
        this->Controls->Add(this->labelStats);
        this->Controls->Add(this->statusStrip);

        this->ClientSize = System::Drawing::Size(684, 567);
        this->Text = L"Expense Tracker";
        this->StartPosition = FormStartPosition::CenterScreen;
        this->ResumeLayout(false);
        this->PerformLayout();
    }

    void Form1::ApplyTheme(bool dark) {
        Color backColor, foreColor, controlBackColor, buttonBackColor;

        if (dark) {
            backColor = Color::FromArgb(32, 32, 32);
            foreColor = Color::White;
            controlBackColor = Color::FromArgb(45, 45, 48);
            buttonBackColor = Color::FromArgb(70, 70, 75);
            this->btnTheme->Text = L"Светлая тема";
        }
        else {
            backColor = Color::FromArgb(240, 240, 240);
            foreColor = Color::FromArgb(32, 32, 32);
            controlBackColor = Color::White;
            buttonBackColor = Color::FromArgb(230, 230, 230);
            this->btnTheme->Text = L"Темная тема";
        }

        this->BackColor = backColor;
        this->ForeColor = foreColor;

        for each (Control ^ control in this->Controls) {
            if (dynamic_cast<Label^>(control) && control != labelTotal && control != labelIncome &&
                control != labelExpense && control != labelBalance && control != labelStats) {
                control->ForeColor = foreColor;
                control->BackColor = backColor;
            }
            else if (dynamic_cast<TextBox^>(control) || dynamic_cast<ComboBox^>(control) || dynamic_cast<DateTimePicker^>(control)) {
                control->BackColor = controlBackColor;
                control->ForeColor = foreColor;
                if (dynamic_cast<ComboBox^>(control)) {
                    safe_cast<ComboBox^>(control)->FlatStyle = FlatStyle::Flat;
                }
                if (dynamic_cast<TextBox^>(control)) {
                    safe_cast<TextBox^>(control)->BorderStyle = BorderStyle::FixedSingle;
                }
            }
            else if (dynamic_cast<Button^>(control)) {
                control->BackColor = buttonBackColor;
                control->ForeColor = foreColor;
                safe_cast<Button^>(control)->FlatStyle = FlatStyle::Flat;
            }
            else if (dynamic_cast<ListView^>(control)) {
                control->BackColor = controlBackColor;
                control->ForeColor = foreColor;
            }
        }

        this->statusStrip->BackColor = buttonBackColor;
        this->statusStrip->ForeColor = foreColor;
        this->statusLabel->ForeColor = foreColor;

        this->labelTotal->ForeColor = foreColor;
        this->labelBalance->ForeColor = foreColor;
        this->labelStats->ForeColor = foreColor;
    }

    void Form1::btnAdd_Click(Object^ sender, EventArgs^ e) {
        try {
            String^ desc = textBoxDescription->Text;
            String^ amtStr = textBoxAmount->Text;

            if (String::IsNullOrWhiteSpace(desc) || String::IsNullOrWhiteSpace(amtStr)) {
                MessageBox::Show(L"Заполните все поля!", L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            double amt = Double::Parse(amtStr);
            if (amt <= 0) {
                MessageBox::Show(L"Сумма должна быть > 0!", L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            std::string description = msclr::interop::marshal_as<std::string>(desc);
            Category cat = (Category)comboBoxCategory->SelectedIndex;
            TransactionType type = (TransactionType)comboBoxType->SelectedIndex;
            time_t selectedDate = DateTimeToTimeT(dateTimePickerExpense->Value);

            manager->addTransaction(description, amt, cat, type, selectedDate);

            textBoxDescription->Clear();
            textBoxAmount->Clear();
            comboBoxCategory->SelectedIndex = 0;
            comboBoxType->SelectedIndex = 0;
            dateTimePickerExpense->Value = DateTime::Now;

            RefreshTransactionList();
            UpdateTotals();
            statusLabel->Text = L"Запись успешно добавлена";
        }
        catch (FormatException^) {
            MessageBox::Show(L"Введите корректную сумму!", L"Ошибка",
                MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
        catch (Exception^ ex) {
            MessageBox::Show(L"Ошибка: " + ex->Message, L"Ошибка",
                MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }

    void Form1::btnDelete_Click(Object^ sender, EventArgs^ e) {
        if (listViewExpenses->SelectedItems->Count == 0) {
            MessageBox::Show(L"Выберите запись для удаления!", L"Ошибка",
                MessageBoxButtons::OK, MessageBoxIcon::Warning);
            return;
        }

        if (MessageBox::Show(L"Вы уверены, что хотите удалить эту запись?", L"Подтверждение удаления",
            MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {
            int id = Convert::ToInt32(listViewExpenses->SelectedItems[0]->SubItems[0]->Text);
            manager->deleteTransaction(id);
            RefreshTransactionList();
            UpdateTotals();
            selectedTransactionId = -1;
            textBoxDescription->Clear();
            textBoxAmount->Clear();
            statusLabel->Text = L"Запись успешно удалена";
        }
    }

    void Form1::btnUpdate_Click(Object^ sender, EventArgs^ e) {
        if (selectedTransactionId == -1) {
            MessageBox::Show(L"Выберите запись для обновления!", L"Ошибка",
                MessageBoxButtons::OK, MessageBoxIcon::Warning);
            return;
        }

        try {
            String^ desc = textBoxDescription->Text;
            String^ amtStr = textBoxAmount->Text;

            if (String::IsNullOrWhiteSpace(desc) || String::IsNullOrWhiteSpace(amtStr)) {
                MessageBox::Show(L"Заполните все поля!", L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            double amt = Double::Parse(amtStr);
            if (amt <= 0) {
                MessageBox::Show(L"Сумма должна быть > 0!", L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            std::string description = msclr::interop::marshal_as<std::string>(desc);
            Category cat = (Category)comboBoxCategory->SelectedIndex;
            TransactionType type = (TransactionType)comboBoxType->SelectedIndex;
            time_t selectedDate = DateTimeToTimeT(dateTimePickerExpense->Value);

            manager->updateTransaction(selectedTransactionId, description, amt, cat, type, selectedDate);

            RefreshTransactionList();
            UpdateTotals();
            selectedTransactionId = -1;
            textBoxDescription->Clear();
            textBoxAmount->Clear();
            comboBoxCategory->SelectedIndex = 0;
            comboBoxType->SelectedIndex = 0;
            statusLabel->Text = L"Запись успешно обновлена";
        }
        catch (FormatException^) {
            MessageBox::Show(L"Введите корректную сумму!", L"Ошибка",
                MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
        catch (Exception^ ex) {
            MessageBox::Show(L"Ошибка: " + ex->Message, L"Ошибка",
                MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }

    void Form1::btnFilter_Click(Object^ sender, EventArgs^ e) {
        comboBoxFilter->SelectedIndex = 0;
        comboBoxFilterType->SelectedIndex = 0;
        currentFilterCategory = L"Все категории";
        currentFilterType = L"Все типы";
        RefreshTransactionList();
        UpdateTotals();
        statusLabel->Text = L"Фильтры сброшены";
    }

    void Form1::btnTheme_Click(Object^ sender, EventArgs^ e) {
        darkThemeEnabled = !darkThemeEnabled;
        ApplyTheme(darkThemeEnabled);
    }

    void Form1::comboBoxFilter_SelectedIndexChanged(Object^ sender, EventArgs^ e) {
        currentFilterCategory = comboBoxFilter->SelectedItem->ToString();
        RefreshTransactionList();
        UpdateTotals();
    }

    void Form1::comboBoxFilterType_SelectedIndexChanged(Object^ sender, EventArgs^ e) {
        currentFilterType = comboBoxFilterType->SelectedItem->ToString();
        RefreshTransactionList();
        UpdateTotals();
    }

    void Form1::listViewExpenses_SelectedIndexChanged(Object^ sender, EventArgs^ e) {
        if (listViewExpenses->SelectedItems->Count > 0) {
            ListViewItem^ selectedItem = listViewExpenses->SelectedItems[0];
            selectedTransactionId = Convert::ToInt32(selectedItem->SubItems[0]->Text);
            textBoxDescription->Text = selectedItem->SubItems[1]->Text;
            textBoxAmount->Text = selectedItem->SubItems[2]->Text->Replace(L"₽", L"")->Replace(L"руб.", L"")->Trim();

            String^ categoryText = selectedItem->SubItems[3]->Text;
            for (int i = 0; i < comboBoxCategory->Items->Count; i++) {
                if (comboBoxCategory->Items[i]->ToString() == categoryText) {
                    comboBoxCategory->SelectedIndex = i;
                    break;
                }
            }

            String^ typeText = selectedItem->SubItems[4]->Text;
            for (int i = 0; i < comboBoxType->Items->Count; i++) {
                if (comboBoxType->Items[i]->ToString() == typeText) {
                    comboBoxType->SelectedIndex = i;
                    break;
                }
            }
        }
    }

    void Form1::listViewExpenses_MouseDoubleClick(Object^ sender, MouseEventArgs^ e) {
        if (listViewExpenses->SelectedItems->Count > 0) {
            ListViewItem^ selectedItem = listViewExpenses->SelectedItems[0];
            int id = Convert::ToInt32(selectedItem->SubItems[0]->Text);
            String^ description = selectedItem->SubItems[1]->Text;
            String^ amount = selectedItem->SubItems[2]->Text->Replace(L"₽", L"")->Replace(L"руб.", L"")->Trim();
            String^ categoryText = selectedItem->SubItems[3]->Text;
            String^ typeText = selectedItem->SubItems[4]->Text;

            Category cat = Category::OTHER;
            if (categoryText == L"Еда") cat = Category::FOOD;
            else if (categoryText == L"Транспорт") cat = Category::TRANSPORT;
            else if (categoryText == L"Развлечения") cat = Category::ENTERTAINMENT;
            else if (categoryText == L"Здоровье") cat = Category::HEALTH;
            else if (categoryText == L"Покупки") cat = Category::SHOPPING;

            TransactionType type = TransactionType::EXPENSE;
            if (typeText == L"Доход") type = TransactionType::INCOME;

            EditForm^ editForm = gcnew EditForm();
            editForm->SetData(description, amount, cat, type);

            if (editForm->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                try {
                    String^ newDesc = editForm->GetDescription();
                    String^ newAmtStr = editForm->GetAmount();
                    Category newCat = editForm->GetCategory();
                    TransactionType newType = editForm->GetTransactionType();

                    if (String::IsNullOrWhiteSpace(newDesc) || String::IsNullOrWhiteSpace(newAmtStr)) {
                        MessageBox::Show(L"Заполните все поля!", L"Ошибка",
                            MessageBoxButtons::OK, MessageBoxIcon::Warning);
                        return;
                    }

                    double newAmt = Double::Parse(newAmtStr);
                    if (newAmt <= 0) {
                        MessageBox::Show(L"Сумма должна быть > 0!", L"Ошибка",
                            MessageBoxButtons::OK, MessageBoxIcon::Warning);
                        return;
                    }

                    std::string descriptionStr = msclr::interop::marshal_as<std::string>(newDesc);
                    time_t selectedDate = DateTimeToTimeT(dateTimePickerExpense->Value);

                    manager->updateTransaction(id, descriptionStr, newAmt, newCat, newType, selectedDate);

                    RefreshTransactionList();
                    UpdateTotals();
                    selectedTransactionId = -1;
                    textBoxDescription->Clear();
                    textBoxAmount->Clear();
                    statusLabel->Text = L"Запись успешно обновлена";
                }
                catch (FormatException^) {
                    MessageBox::Show(L"Введите корректную сумму!", L"Ошибка",
                        MessageBoxButtons::OK, MessageBoxIcon::Error);
                }
                catch (Exception^ ex) {
                    MessageBox::Show(L"Ошибка: " + ex->Message, L"Ошибка",
                        MessageBoxButtons::OK, MessageBoxIcon::Error);
                }
            }
        }
    }

    void Form1::listViewExpenses_ColumnClick(Object^ sender, ColumnClickEventArgs^ e) {
        if (e->Column == lastSortedColumn) {
            sortAscending = !sortAscending;
        }
        else {
            lastSortedColumn = e->Column;
            sortAscending = true;
        }

        this->listViewExpenses->ListViewItemSorter = gcnew ListViewItemComparer(e->Column, sortAscending);
        this->listViewExpenses->Sort();
    }

    void Form1::RefreshTransactionList() {
        listViewExpenses->Items->Clear();
        std::vector<Expense>& transactions = manager->getTransactions();

        for (const auto& t : transactions) {
            String^ cat = gcnew String(manager->categoryToDisplayName(t.getCategory()).c_str());
            String^ type = gcnew String(manager->typeToDisplayName(t.getType()).c_str());

            bool categoryMatch = currentFilterCategory == L"Все категории" || currentFilterCategory == cat;
            bool typeMatch = currentFilterType == L"Все типы" || currentFilterType == type;

            if (categoryMatch && typeMatch) {
                ListViewItem^ item = gcnew ListViewItem(t.getId().ToString());
                item->SubItems->Add(gcnew String(t.getDescription().c_str()));
                item->SubItems->Add(FormatAmount(t.getAmount()) + L" ₽");
                item->SubItems->Add(cat);
                item->SubItems->Add(type);
                item->SubItems->Add(FormatDate(t.getDate()));

                if (t.getType() == TransactionType::INCOME) {
                    item->ForeColor = Color::FromArgb(76, 175, 80);
                }
                else {
                    item->ForeColor = Color::FromArgb(244, 67, 54);
                }

                listViewExpenses->Items->Add(item);
            }
        }

        if (lastSortedColumn >= 0) {
            this->listViewExpenses->ListViewItemSorter = gcnew ListViewItemComparer(lastSortedColumn, sortAscending);
            this->listViewExpenses->Sort();
        }
    }

    void Form1::UpdateTotals() {
        std::vector<Expense>& transactions = manager->getTransactions();
        double totalIncome = 0;
        double totalExpense = 0;

        for (const auto& t : transactions) {
            String^ cat = gcnew String(manager->categoryToDisplayName(t.getCategory()).c_str());
            String^ type = gcnew String(manager->typeToDisplayName(t.getType()).c_str());

            bool categoryMatch = currentFilterCategory == L"Все категории" || currentFilterCategory == cat;
            bool typeMatch = currentFilterType == L"Все типы" || currentFilterType == type;

            if (categoryMatch && typeMatch) {
                if (t.getType() == TransactionType::INCOME) {
                    totalIncome += t.getAmount();
                }
                else {
                    totalExpense += t.getAmount();
                }
            }
        }

        double balance = totalIncome - totalExpense;

        labelTotal->Text = String::Format(L"Всего записей: {0}", transactions.size());
        labelIncome->Text = String::Format(L"Доходы: {0:F2} ₽", totalIncome);
        labelExpense->Text = String::Format(L"Расходы: {0:F2} ₽", totalExpense);
        labelBalance->Text = String::Format(L"Баланс: {0:F2} ₽", balance);

        if (balance >= 0) {
            labelBalance->ForeColor = Color::FromArgb(76, 175, 80);
        }
        else {
            labelBalance->ForeColor = Color::FromArgb(244, 67, 54);
        }

        cli::array<String^>^ categories = gcnew cli::array<String^> { L"Еда", L"Транспорт", L"Развлечения", L"Здоровье", L"Покупки", L"Другое" };

        Dictionary<String^, double>^ categoryIncome = gcnew Dictionary<String^, double>();
        Dictionary<String^, double>^ categoryExpense = gcnew Dictionary<String^, double>();

        for each (String ^ cat in categories) {
            categoryIncome->Add(cat, 0.0);
            categoryExpense->Add(cat, 0.0);
        }

        for (const auto& t : transactions) {
            String^ cat = gcnew String(manager->categoryToDisplayName(t.getCategory()).c_str());
            if (t.getType() == TransactionType::INCOME) {
                double val = categoryIncome[cat];
                categoryIncome[cat] = val + t.getAmount();
            }
            else {
                double val = categoryExpense[cat];
                categoryExpense[cat] = val + t.getAmount();
            }
        }

        String^ statsText = L"Доходы: ";
        bool first = true;

        for each (String ^ cat in categories) {
            if (categoryIncome[cat] > 0) {
                if (!first) statsText += L" | ";
                statsText += String::Format(L"{0}: {1:F2} ₽", cat, categoryIncome[cat]);
                first = false;
            }
        }

        if (first) statsText += L"нет";

        statsText += L"    Расходы: ";
        first = true;

        for each (String ^ cat in categories) {
            if (categoryExpense[cat] > 0) {
                if (!first) statsText += L" | ";
                statsText += String::Format(L"{0}: {1:F2} ₽", cat, categoryExpense[cat]);
                first = false;
            }
        }

        if (first) statsText += L"нет";

        labelStats->Text = statsText;
    }

    String^ Form1::FormatDate(time_t time) {
        struct tm timeinfo;
        localtime_s(&timeinfo, &time);
        wchar_t buffer[20];
        wcsftime(buffer, 20, L"%d.%m.%Y", &timeinfo);
        return gcnew String(buffer);
    }

    String^ Form1::FormatAmount(double amount) {
        return String::Format(L"{0:F2}", amount);
    }

    time_t Form1::DateTimeToTimeT(DateTime dt) {
        struct tm tmval = {};
        tmval.tm_year = dt.Year - 1900;
        tmval.tm_mon = dt.Month - 1;
        tmval.tm_mday = dt.Day;
        tmval.tm_hour = dt.Hour;
        tmval.tm_min = dt.Minute;
        tmval.tm_sec = dt.Second;
        tmval.tm_isdst = -1;
        return mktime(&tmval);
    }
}
