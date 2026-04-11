#include "Form1.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Collections::Generic;

namespace ExpenseTrackerApp {

    public ref class ListViewItemComparer : public System::Collections::IComparer {
    private:
        int col;
        bool ascending;
    public:
        ListViewItemComparer(int column, bool asc) : col(column), ascending(asc) {}
        virtual int Compare(Object^ x, Object^ y) {
            ListViewItem^ a = safe_cast<ListViewItem^>(x);
            ListViewItem^ b = safe_cast<ListViewItem^>(y);
            String^ strA = a->SubItems[col]->Text;
            String^ strB = b->SubItems[col]->Text;
            int result;
            if (col == 0 || col == 2) {
                double valA = Double::Parse(strA->Replace(L"₽", "")->Trim());
                double valB = Double::Parse(strB->Replace(L"₽", "")->Trim());
                result = valA.CompareTo(valB);
            }
            else if (col == 5) {
                DateTime dtA = DateTime::ParseExact(strA, L"dd.MM.yyyy", nullptr);
                DateTime dtB = DateTime::ParseExact(strB, L"dd.MM.yyyy", nullptr);
                result = DateTime::Compare(dtA, dtB);
            }
            else {
                result = String::Compare(strA, strB);
            }
            return ascending ? result : -result;
        }
    };

    Form1::Form1() {
        manager = new ExpenseManager();
        darkThemeEnabled = true;
        monthlyBudget = 0;
        currentMonthExpenses = 0;
        selectedTransactionId = -1;
        currentFilterCategory = L"Все";
        currentFilterType = L"Все";
        lastSortedColumn = -1;
        sortAscending = true;

        InitializeComponent();
        LoadBudget();
        RefreshTransactionList();
        UpdateTotals();
        ApplyTheme(darkThemeEnabled);
        alertTimer->Start();
    }

    Form1::~Form1() {
        SaveBudget();
        delete manager;
        if (components) delete components;
    }

    void Form1::InitializeComponent() {
        this->components = gcnew System::ComponentModel::Container();

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
        this->btnReport = gcnew Button();
        this->btnBudget = gcnew Button();
        this->btnQuickAdd = gcnew Button();
        this->listViewExpenses = gcnew ListView();
        this->labelTotal = gcnew Label();
        this->labelIncome = gcnew Label();
        this->labelExpense = gcnew Label();
        this->labelBalance = gcnew Label();
        this->labelStats = gcnew Label();
        this->labelBudgetAlert = gcnew Label();
        this->statusStrip = gcnew StatusStrip();
        this->statusLabel = gcnew ToolStripStatusLabel();
        this->alertTimer = gcnew Timer(this->components);

        this->SuspendLayout();

        this->textBoxDescription->Location = Point(12, 12);
        this->textBoxDescription->Size = System::Drawing::Size(200, 23);

        this->textBoxAmount->Location = Point(222, 12);
        this->textBoxAmount->Size = System::Drawing::Size(100, 23);

        this->comboBoxCategory->DropDownStyle = ComboBoxStyle::DropDownList;
        this->comboBoxCategory->Items->AddRange(gcnew cli::array<Object^> {
            L"Еда", L"Транспорт", L"Развлечения", L"Здоровье", L"Покупки", L"Другое"
        });
        this->comboBoxCategory->Location = Point(332, 12);
        this->comboBoxCategory->Size = System::Drawing::Size(120, 23);
        this->comboBoxCategory->SelectedIndex = 0;

        this->comboBoxType->DropDownStyle = ComboBoxStyle::DropDownList;
        this->comboBoxType->Items->AddRange(gcnew cli::array<Object^> { L"Расход", L"Доход" });
        this->comboBoxType->Location = Point(462, 12);
        this->comboBoxType->Size = System::Drawing::Size(100, 23);
        this->comboBoxType->SelectedIndex = 0;

        this->dateTimePickerExpense->Location = Point(572, 12);
        this->dateTimePickerExpense->Size = System::Drawing::Size(140, 23);
        this->dateTimePickerExpense->Format = DateTimePickerFormat::Short;

        this->btnAdd->Location = Point(722, 10);
        this->btnAdd->Size = System::Drawing::Size(75, 23);
        this->btnAdd->Text = L"Добавить";
        this->btnAdd->Click += gcnew EventHandler(this, &Form1::btnAdd_Click);

        this->btnUpdate->Location = Point(807, 10);
        this->btnUpdate->Size = System::Drawing::Size(75, 23);
        this->btnUpdate->Text = L"Изменить";
        this->btnUpdate->Enabled = false;
        this->btnUpdate->Click += gcnew EventHandler(this, &Form1::btnUpdate_Click);

        this->btnDelete->Location = Point(892, 10);
        this->btnDelete->Size = System::Drawing::Size(75, 23);
        this->btnDelete->Text = L"Удалить";
        this->btnDelete->Enabled = false;
        this->btnDelete->Click += gcnew EventHandler(this, &Form1::btnDelete_Click);

        this->comboBoxFilter->DropDownStyle = ComboBoxStyle::DropDownList;
        this->comboBoxFilter->Items->AddRange(gcnew cli::array<Object^> {
            L"Все", L"Еда", L"Транспорт", L"Развлечения", L"Здоровье", L"Покупки", L"Другое"
        });
        this->comboBoxFilter->Location = Point(12, 45);
        this->comboBoxFilter->Size = System::Drawing::Size(120, 23);
        this->comboBoxFilter->SelectedIndex = 0;
        this->comboBoxFilter->SelectedIndexChanged += gcnew EventHandler(this, &Form1::comboBoxFilter_SelectedIndexChanged);

        this->comboBoxFilterType->DropDownStyle = ComboBoxStyle::DropDownList;
        this->comboBoxFilterType->Items->AddRange(gcnew cli::array<Object^> { L"Все", L"Расход", L"Доход" });
        this->comboBoxFilterType->Location = Point(142, 45);
        this->comboBoxFilterType->Size = System::Drawing::Size(100, 23);
        this->comboBoxFilterType->SelectedIndex = 0;
        this->comboBoxFilterType->SelectedIndexChanged += gcnew EventHandler(this, &Form1::comboBoxFilterType_SelectedIndexChanged);

        this->btnFilter->Location = Point(252, 43);
        this->btnFilter->Size = System::Drawing::Size(75, 23);
        this->btnFilter->Text = L"Сбросить";
        this->btnFilter->Click += gcnew EventHandler(this, &Form1::btnFilter_Click);

        this->btnTheme->Location = Point(337, 43);
        this->btnTheme->Size = System::Drawing::Size(100, 23);
        this->btnTheme->Text = L"Светлая тема";
        this->btnTheme->Click += gcnew EventHandler(this, &Form1::btnTheme_Click);

        this->btnReport->Location = Point(447, 43);
        this->btnReport->Size = System::Drawing::Size(100, 23);
        this->btnReport->Text = L"Отчёт";
        this->btnReport->Click += gcnew EventHandler(this, &Form1::btnReport_Click);

        this->btnBudget->Location = Point(557, 43);
        this->btnBudget->Size = System::Drawing::Size(100, 23);
        this->btnBudget->Text = L"Бюджет";
        this->btnBudget->Click += gcnew EventHandler(this, &Form1::btnBudget_Click);

        this->btnQuickAdd->Location = Point(667, 43);
        this->btnQuickAdd->Size = System::Drawing::Size(120, 23);
        this->btnQuickAdd->Text = L"Быстро: Еда 500₽";
        this->btnQuickAdd->Click += gcnew EventHandler(this, &Form1::btnQuickAdd_Click);

        this->listViewExpenses->Location = Point(12, 75);
        this->listViewExpenses->Size = System::Drawing::Size(950, 350);
        this->listViewExpenses->View = View::Details;
        this->listViewExpenses->FullRowSelect = true;
        this->listViewExpenses->GridLines = true;
        this->listViewExpenses->Columns->Add(L"ID", 40);
        this->listViewExpenses->Columns->Add(L"Описание", 200);
        this->listViewExpenses->Columns->Add(L"Сумма", 100, HorizontalAlignment::Right);
        this->listViewExpenses->Columns->Add(L"Категория", 120);
        this->listViewExpenses->Columns->Add(L"Тип", 80);
        this->listViewExpenses->Columns->Add(L"Дата", 120);
        this->listViewExpenses->SelectedIndexChanged += gcnew EventHandler(this, &Form1::listViewExpenses_SelectedIndexChanged);
        this->listViewExpenses->ColumnClick += gcnew ColumnClickEventHandler(this, &Form1::listViewExpenses_ColumnClick);
        this->listViewExpenses->MouseDoubleClick += gcnew MouseEventHandler(this, &Form1::listViewExpenses_MouseDoubleClick);

        this->labelTotal->Location = Point(12, 435);
        this->labelTotal->Size = System::Drawing::Size(200, 23);

        this->labelIncome->Location = Point(12, 465);
        this->labelIncome->Size = System::Drawing::Size(200, 23);

        this->labelExpense->Location = Point(220, 435);
        this->labelExpense->Size = System::Drawing::Size(200, 23);

        this->labelBalance->Location = Point(220, 465);
        this->labelBalance->Size = System::Drawing::Size(200, 23);

        this->labelStats->Location = Point(430, 435);
        this->labelStats->Size = System::Drawing::Size(300, 50);

        this->labelBudgetAlert->Location = Point(750, 435);
        this->labelBudgetAlert->Size = System::Drawing::Size(200, 50);
        this->labelBudgetAlert->ForeColor = Color::Red;
        this->labelBudgetAlert->Visible = false;

        this->statusStrip->Location = Point(0, 500);
        this->statusStrip->Size = System::Drawing::Size(980, 22);
        this->statusStrip->Items->Add(this->statusLabel);

        this->AutoScaleDimensions = System::Drawing::SizeF(7, 15);
        this->ClientSize = System::Drawing::Size(980, 522);
        this->Controls->Add(this->textBoxDescription);
        this->Controls->Add(this->textBoxAmount);
        this->Controls->Add(this->comboBoxCategory);
        this->Controls->Add(this->comboBoxType);
        this->Controls->Add(this->dateTimePickerExpense);
        this->Controls->Add(this->btnAdd);
        this->Controls->Add(this->btnUpdate);
        this->Controls->Add(this->btnDelete);
        this->Controls->Add(this->comboBoxFilter);
        this->Controls->Add(this->comboBoxFilterType);
        this->Controls->Add(this->btnFilter);
        this->Controls->Add(this->btnTheme);
        this->Controls->Add(this->btnReport);
        this->Controls->Add(this->btnBudget);
        this->Controls->Add(this->btnQuickAdd);
        this->Controls->Add(this->listViewExpenses);
        this->Controls->Add(this->labelTotal);
        this->Controls->Add(this->labelIncome);
        this->Controls->Add(this->labelExpense);
        this->Controls->Add(this->labelBalance);
        this->Controls->Add(this->labelStats);
        this->Controls->Add(this->labelBudgetAlert);
        this->Controls->Add(this->statusStrip);

        this->Text = L"Expense Tracker";
        this->StartPosition = FormStartPosition::CenterScreen;
        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
        this->MaximizeBox = false;

        this->alertTimer->Interval = 300000;
        this->alertTimer->Tick += gcnew EventHandler(this, &Form1::alertTimer_Tick);

        this->ResumeLayout(false);
        this->PerformLayout();
    }

    void Form1::RefreshTransactionList() {
        listViewExpenses->Items->Clear();
        auto& transactions = manager->getTransactions();

        for (auto& t : transactions) {
            if (currentFilterCategory != L"Все") {
                String^ catStr = gcnew String(manager->categoryToDisplayName(t.getCategory()).c_str());
                if (catStr != currentFilterCategory) continue;
            }
            if (currentFilterType != L"Все") {
                String^ typeStr = gcnew String(manager->typeToDisplayName(t.getType()).c_str());
                if (typeStr != currentFilterType) continue;
            }

            ListViewItem^ item = gcnew ListViewItem(t.getId().ToString());
            item->SubItems->Add(gcnew String(t.getDescription().c_str()));
            item->SubItems->Add(FormatAmount(t.getAmount()));
            item->SubItems->Add(gcnew String(manager->categoryToDisplayName(t.getCategory()).c_str()));
            item->SubItems->Add(gcnew String(manager->typeToDisplayName(t.getType()).c_str()));
            item->SubItems->Add(FormatDate(t.getDate()));
            listViewExpenses->Items->Add(item);
        }
        UpdateTotals();
    }

    void Form1::UpdateTotals() {
        double totalIncome = manager->getTotalByType(TransactionType::INCOME);
        double totalExpense = manager->getTotalByType(TransactionType::EXPENSE);
        double balance = totalIncome - totalExpense;

        labelIncome->Text = String::Format(L"Доходы: {0:F2} ₽", totalIncome);
        labelExpense->Text = String::Format(L"Расходы: {0:F2} ₽", totalExpense);
        labelBalance->Text = String::Format(L"Баланс: {0:F2} ₽", balance);
        labelBalance->ForeColor = balance >= 0 ? Color::Green : Color::Red;

        time_t now = time(nullptr);
        struct tm tm_now;
        localtime_s(&tm_now, &now);
        currentMonthExpenses = 0;
        double monthIncome = 0;
        for (auto& t : manager->getTransactions()) {
            struct tm tm_t;
            time_t t_date = t.getDate();
            localtime_s(&tm_t, &t_date);
            if (tm_t.tm_mon == tm_now.tm_mon && tm_t.tm_year == tm_now.tm_year) {
                if (t.getType() == TransactionType::EXPENSE)
                    currentMonthExpenses += t.getAmount();
                else
                    monthIncome += t.getAmount();
            }
        }
        labelStats->Text = String::Format(L"За месяц: доход {0:F2} ₽, расход {1:F2} ₽", monthIncome, currentMonthExpenses);
        UpdateBudgetAlert();
    }

    void Form1::UpdateBudgetAlert() {
        if (monthlyBudget > 0) {
            double percent = (currentMonthExpenses / monthlyBudget) * 100;
            labelBudgetAlert->Text = String::Format(L"Бюджет: {0:F2} / {1:F2} ₽ ({2:F1}%)",
                currentMonthExpenses, monthlyBudget, percent);
            labelBudgetAlert->Visible = true;
            if (percent >= 90) {
                labelBudgetAlert->ForeColor = Color::Red;
                labelBudgetAlert->Text += L"\nПРЕВЫШЕНИЕ БЮДЖЕТА!";
            }
            else if (percent >= 75) {
                labelBudgetAlert->ForeColor = Color::Orange;
            }
            else {
                labelBudgetAlert->ForeColor = Color::Yellow;
            }
        }
        else {
            labelBudgetAlert->Visible = false;
        }
    }

    void Form1::LoadBudget() {
        try {
            std::ifstream file("budget.txt");
            if (file.is_open()) {
                double temp = 0.0;
                file >> temp;
                monthlyBudget = temp;
                file.close();
            }
        }
        catch (...) {}
    }

    void Form1::SaveBudget() {
        try {
            std::ofstream file("budget.txt");
            if (file.is_open()) {
                file << monthlyBudget;
                file.close();
            }
        }
        catch (...) {}
    }

    String^ Form1::FormatDate(time_t time) {
        struct tm tm_buf;
        localtime_s(&tm_buf, &time);
        wchar_t buffer[80];
        wcsftime(buffer, 80, L"%d.%m.%Y", &tm_buf);
        return gcnew String(buffer);
    }

    String^ Form1::FormatAmount(double amount) {
        return String::Format(L"{0:F2} ₽", amount);
    }

    void Form1::btnAdd_Click(Object^ sender, EventArgs^ e) {
        if (String::IsNullOrWhiteSpace(textBoxDescription->Text) ||
            String::IsNullOrWhiteSpace(textBoxAmount->Text)) {
            MessageBox::Show(L"Заполните описание и сумму!", L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
            return;
        }
        try {
            double amount = Double::Parse(textBoxAmount->Text);
            if (amount <= 0) {
                MessageBox::Show(L"Сумма должна быть положительной!", L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }
            std::string desc = msclr::interop::marshal_as<std::string>(textBoxDescription->Text);
            Category cat = static_cast<Category>(comboBoxCategory->SelectedIndex);
            TransactionType type = static_cast<TransactionType>(comboBoxType->SelectedIndex);
            time_t date = DateTimeToTimeT(dateTimePickerExpense->Value);

            manager->addTransaction(desc, amount, cat, type, date);
            RefreshTransactionList();
            textBoxDescription->Clear();
            textBoxAmount->Clear();
            comboBoxCategory->SelectedIndex = 0;
            comboBoxType->SelectedIndex = 0;
            dateTimePickerExpense->Value = DateTime::Now;
            statusLabel->Text = L"Запись добавлена";
        }
        catch (FormatException^) {
            MessageBox::Show(L"Некорректная сумма!", L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }

    void Form1::btnUpdate_Click(Object^ sender, EventArgs^ e) {
        if (selectedTransactionId == -1) return;
        if (String::IsNullOrWhiteSpace(textBoxDescription->Text) ||
            String::IsNullOrWhiteSpace(textBoxAmount->Text)) {
            MessageBox::Show(L"Заполните все поля!", L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
            return;
        }
        try {
            double amount = Double::Parse(textBoxAmount->Text);
            if (amount <= 0) {
                MessageBox::Show(L"Сумма должна быть положительной!", L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }
            std::string desc = msclr::interop::marshal_as<std::string>(textBoxDescription->Text);
            Category cat = static_cast<Category>(comboBoxCategory->SelectedIndex);
            TransactionType type = static_cast<TransactionType>(comboBoxType->SelectedIndex);
            time_t date = DateTimeToTimeT(dateTimePickerExpense->Value);

            manager->updateTransaction(selectedTransactionId, desc, amount, cat, type, date);
            RefreshTransactionList();
            textBoxDescription->Clear();
            textBoxAmount->Clear();
            btnUpdate->Enabled = false;
            btnDelete->Enabled = false;
            btnAdd->Enabled = true;
            selectedTransactionId = -1;
            statusLabel->Text = L"Запись обновлена";
        }
        catch (FormatException^) {
            MessageBox::Show(L"Некорректная сумма!", L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }

    void Form1::btnDelete_Click(Object^ sender, EventArgs^ e) {
        if (selectedTransactionId == -1) return;
        if (MessageBox::Show(L"Удалить запись?", L"Подтверждение",
            MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {
            manager->deleteTransaction(selectedTransactionId);
            RefreshTransactionList();
            textBoxDescription->Clear();
            textBoxAmount->Clear();
            btnUpdate->Enabled = false;
            btnDelete->Enabled = false;
            btnAdd->Enabled = true;
            selectedTransactionId = -1;
            statusLabel->Text = L"Запись удалена";
        }
    }

    void Form1::btnFilter_Click(Object^ sender, EventArgs^ e) {
        currentFilterCategory = L"Все";
        currentFilterType = L"Все";
        comboBoxFilter->SelectedIndex = 0;
        comboBoxFilterType->SelectedIndex = 0;
        RefreshTransactionList();
    }

    void Form1::btnTheme_Click(Object^ sender, EventArgs^ e) {
        darkThemeEnabled = !darkThemeEnabled;
        ApplyTheme(darkThemeEnabled);
        btnTheme->Text = darkThemeEnabled ? L"Светлая тема" : L"Тёмная тема";
    }

    void Form1::btnReport_Click(Object^ sender, EventArgs^ e) {
        ReportForm^ reportForm = gcnew ReportForm(manager);
        reportForm->ShowDialog();
    }

    void Form1::btnBudget_Click(Object^ sender, EventArgs^ e) {
        BudgetForm^ budgetForm = gcnew BudgetForm(monthlyBudget);
        if (budgetForm->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            monthlyBudget = budgetForm->GetBudget();
            SaveBudget();
            UpdateBudgetAlert();
        }
    }

    void Form1::btnQuickAdd_Click(Object^ sender, EventArgs^ e) {
        time_t now = time(nullptr);
        manager->addTransaction("Быстрый расход", 500.0, Category::FOOD, TransactionType::EXPENSE, now);
        RefreshTransactionList();
        statusLabel->Text = L"Быстрый расход добавлен";
    }

    void Form1::comboBoxFilter_SelectedIndexChanged(Object^ sender, EventArgs^ e) {
        currentFilterCategory = comboBoxFilter->Text;
        RefreshTransactionList();
    }

    void Form1::comboBoxFilterType_SelectedIndexChanged(Object^ sender, EventArgs^ e) {
        currentFilterType = comboBoxFilterType->Text;
        RefreshTransactionList();
    }

    void Form1::listViewExpenses_SelectedIndexChanged(Object^ sender, EventArgs^ e) {
        if (listViewExpenses->SelectedItems->Count > 0) {
            ListViewItem^ item = listViewExpenses->SelectedItems[0];
            selectedTransactionId = Int32::Parse(item->Text);
            textBoxDescription->Text = item->SubItems[1]->Text;
            String^ amountStr = item->SubItems[2]->Text->Replace(L" ₽", "");
            textBoxAmount->Text = amountStr;

            String^ catStr = item->SubItems[3]->Text;
            for (int i = 0; i < comboBoxCategory->Items->Count; i++) {
                if (comboBoxCategory->Items[i]->ToString() == catStr) {
                    comboBoxCategory->SelectedIndex = i;
                    break;
                }
            }
            String^ typeStr = item->SubItems[4]->Text;
            comboBoxType->SelectedIndex = (typeStr == L"Доход") ? 1 : 0;

            DateTime dt = DateTime::ParseExact(item->SubItems[5]->Text, L"dd.MM.yyyy", nullptr);
            dateTimePickerExpense->Value = dt;

            btnUpdate->Enabled = true;
            btnDelete->Enabled = true;
            btnAdd->Enabled = false;
        }
        else {
            btnUpdate->Enabled = false;
            btnDelete->Enabled = false;
            btnAdd->Enabled = true;
            selectedTransactionId = -1;
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

    void Form1::listViewExpenses_MouseDoubleClick(Object^ sender, MouseEventArgs^ e) {
        if (selectedTransactionId != -1) {
            EditForm^ editForm = gcnew EditForm();
            editForm->SetData(textBoxDescription->Text, textBoxAmount->Text,
                static_cast<Category>(comboBoxCategory->SelectedIndex),
                static_cast<TransactionType>(comboBoxType->SelectedIndex));
            if (editForm->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                try {
                    std::string desc = msclr::interop::marshal_as<std::string>(editForm->GetDescription());
                    double amount = Double::Parse(editForm->GetAmount());
                    Category cat = editForm->GetCategory();
                    TransactionType type = editForm->GetTransactionType();
                    time_t date = DateTimeToTimeT(dateTimePickerExpense->Value);
                    manager->updateTransaction(selectedTransactionId, desc, amount, cat, type, date);
                    RefreshTransactionList();
                }
                catch (...) {}
            }
        }
    }

    void Form1::ApplyTheme(bool dark) {
        Color backColor, foreColor, controlBack, listBack;
        if (dark) {
            backColor = Color::FromArgb(32, 32, 32);
            foreColor = Color::White;
            controlBack = Color::FromArgb(45, 45, 48);
            listBack = Color::FromArgb(45, 45, 48);
        }
        else {
            backColor = SystemColors::Control;
            foreColor = SystemColors::ControlText;
            controlBack = SystemColors::Window;
            listBack = SystemColors::Window;
        }
        this->BackColor = backColor;
        this->ForeColor = foreColor;

        for each (Control ^ c in this->Controls) {
            if (dynamic_cast<TextBox^>(c) || dynamic_cast<ComboBox^>(c) || dynamic_cast<DateTimePicker^>(c) || dynamic_cast<ListView^>(c)) {
                c->BackColor = controlBack;
                c->ForeColor = foreColor;
            }
            if (dynamic_cast<Button^>(c)) {
                c->BackColor = dark ? Color::FromArgb(70, 70, 75) : SystemColors::Control;
                c->ForeColor = foreColor;
            }
            if (dynamic_cast<Label^>(c)) {
                c->ForeColor = foreColor;
            }
        }
        listViewExpenses->BackColor = listBack;
        listViewExpenses->ForeColor = foreColor;
    }

    void Form1::alertTimer_Tick(Object^ sender, EventArgs^ e) {
        UpdateBudgetAlert();
    }

    time_t Form1::DateTimeToTimeT(DateTime dt) {
        long long ticks = dt.Ticks - DateTime(1970, 1, 1).Ticks;
        return static_cast<time_t>(ticks / 10000000);
    }

}