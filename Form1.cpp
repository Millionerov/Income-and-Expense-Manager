#using <System.IO.dll>
#include "Form1.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Collections::Generic;
using namespace System::IO;

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
        currentSearchTerm = L"";
        lastSortedColumn = -1;
        sortAscending = true;

        InitializeComponent();
        LoadBudget();
        RefreshTransactionList();
        UpdateTotals();
        ApplyTheme(darkThemeEnabled);
        alertTimer->Start();
        UpdateStatusBar();
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
        this->btnExport = gcnew Button();
        this->textBoxSearch = gcnew TextBox();
        this->listViewExpenses = gcnew ListView();
        this->labelIncome = gcnew Label();
        this->labelExpense = gcnew Label();
        this->labelBalance = gcnew Label();
        this->labelStats = gcnew Label();
        this->labelBudgetAlert = gcnew Label();
        this->labelSearch = gcnew Label();
        this->statusStrip = gcnew StatusStrip();
        this->statusLabel = gcnew ToolStripStatusLabel();
        this->progressBar = gcnew ToolStripProgressBar();
        this->alertTimer = gcnew Timer(this->components);

        this->SuspendLayout();

        this->textBoxDescription->Location = Point(12, 12);
        this->textBoxDescription->Size = System::Drawing::Size(200, 23);
        this->textBoxDescription->Text = L"";

        this->textBoxAmount->Location = Point(222, 12);
        this->textBoxAmount->Size = System::Drawing::Size(100, 23);
        this->textBoxAmount->Text = L"";

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
        this->btnAdd->Size = System::Drawing::Size(75, 27);
        this->btnAdd->Text = L"Добавить";
        this->btnAdd->FlatStyle = FlatStyle::Flat;
        this->btnAdd->Click += gcnew EventHandler(this, &Form1::btnAdd_Click);

        this->btnUpdate->Location = Point(807, 10);
        this->btnUpdate->Size = System::Drawing::Size(75, 27);
        this->btnUpdate->Text = L"Изменить";
        this->btnUpdate->Enabled = false;
        this->btnUpdate->FlatStyle = FlatStyle::Flat;
        this->btnUpdate->Click += gcnew EventHandler(this, &Form1::btnUpdate_Click);

        this->btnDelete->Location = Point(892, 10);
        this->btnDelete->Size = System::Drawing::Size(75, 27);
        this->btnDelete->Text = L"Удалить";
        this->btnDelete->Enabled = false;
        this->btnDelete->FlatStyle = FlatStyle::Flat;
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
        this->btnFilter->Size = System::Drawing::Size(75, 27);
        this->btnFilter->Text = L"Сбросить";
        this->btnFilter->FlatStyle = FlatStyle::Flat;
        this->btnFilter->Click += gcnew EventHandler(this, &Form1::btnFilter_Click);

        this->btnTheme->Location = Point(337, 43);
        this->btnTheme->Size = System::Drawing::Size(100, 27);
        this->btnTheme->Text = L"Светлая тема";
        this->btnTheme->FlatStyle = FlatStyle::Flat;
        this->btnTheme->Click += gcnew EventHandler(this, &Form1::btnTheme_Click);

        this->btnReport->Location = Point(447, 43);
        this->btnReport->Size = System::Drawing::Size(100, 27);
        this->btnReport->Text = L"Отчёт";
        this->btnReport->FlatStyle = FlatStyle::Flat;
        this->btnReport->Click += gcnew EventHandler(this, &Form1::btnReport_Click);

        this->btnBudget->Location = Point(557, 43);
        this->btnBudget->Size = System::Drawing::Size(100, 27);
        this->btnBudget->Text = L"Бюджет";
        this->btnBudget->FlatStyle = FlatStyle::Flat;
        this->btnBudget->Click += gcnew EventHandler(this, &Form1::btnBudget_Click);

        this->btnQuickAdd->Location = Point(667, 43);
        this->btnQuickAdd->Size = System::Drawing::Size(110, 27);
        this->btnQuickAdd->Text = L"Быстрый: Еда 500₽";
        this->btnQuickAdd->FlatStyle = FlatStyle::Flat;
        this->btnQuickAdd->Click += gcnew EventHandler(this, &Form1::btnQuickAdd_Click);

        this->btnExport->Location = Point(787, 43);
        this->btnExport->Size = System::Drawing::Size(80, 27);
        this->btnExport->Text = L"Экспорт";
        this->btnExport->FlatStyle = FlatStyle::Flat;
        this->btnExport->Click += gcnew EventHandler(this, &Form1::btnExport_Click);

        this->labelSearch = gcnew Label();
        this->labelSearch->Text = L"Поиск:";
        this->labelSearch->Location = Point(877, 47);
        this->labelSearch->Size = System::Drawing::Size(40, 20);

        this->textBoxSearch->Location = Point(920, 45);
        this->textBoxSearch->Size = System::Drawing::Size(120, 23);
        this->textBoxSearch->TextChanged += gcnew EventHandler(this, &Form1::textBoxSearch_TextChanged);

        this->listViewExpenses->Location = Point(12, 75);
        this->listViewExpenses->Size = System::Drawing::Size(1030, 380);
        this->listViewExpenses->View = View::Details;
        this->listViewExpenses->FullRowSelect = true;
        this->listViewExpenses->GridLines = true;
        this->listViewExpenses->Columns->Add(L"ID", 45);
        this->listViewExpenses->Columns->Add(L"Описание", 220);
        this->listViewExpenses->Columns->Add(L"Сумма", 110, HorizontalAlignment::Right);
        this->listViewExpenses->Columns->Add(L"Категория", 130);
        this->listViewExpenses->Columns->Add(L"Тип", 90);
        this->listViewExpenses->Columns->Add(L"Дата", 120);
        this->listViewExpenses->SelectedIndexChanged += gcnew EventHandler(this, &Form1::listViewExpenses_SelectedIndexChanged);
        this->listViewExpenses->ColumnClick += gcnew ColumnClickEventHandler(this, &Form1::listViewExpenses_ColumnClick);
        this->listViewExpenses->MouseDoubleClick += gcnew MouseEventHandler(this, &Form1::listViewExpenses_MouseDoubleClick);

        this->labelIncome->Location = Point(12, 465);
        this->labelIncome->Size = System::Drawing::Size(200, 25);
        this->labelIncome->Font = gcnew Drawing::Font(L"Segoe UI", 9, FontStyle::Bold);

        this->labelExpense->Location = Point(220, 465);
        this->labelExpense->Size = System::Drawing::Size(200, 25);
        this->labelExpense->Font = gcnew Drawing::Font(L"Segoe UI", 9, FontStyle::Bold);

        this->labelBalance->Location = Point(430, 465);
        this->labelBalance->Size = System::Drawing::Size(200, 25);
        this->labelBalance->Font = gcnew Drawing::Font(L"Segoe UI", 9, FontStyle::Bold);

        this->labelStats->Location = Point(12, 495);
        this->labelStats->Size = System::Drawing::Size(400, 25);

        this->labelBudgetAlert->Location = Point(640, 465);
        this->labelBudgetAlert->Size = System::Drawing::Size(400, 55);
        this->labelBudgetAlert->Font = gcnew Drawing::Font(L"Segoe UI", 9, FontStyle::Bold);
        this->labelBudgetAlert->Visible = false;

        this->statusStrip->Location = Point(0, 530);
        this->statusStrip->Size = System::Drawing::Size(1060, 24);
        this->statusStrip->Items->Add(this->statusLabel);
        this->statusStrip->Items->Add(this->progressBar);
        this->progressBar->Size = System::Drawing::Size(100, 18);
        this->progressBar->Visible = false;

        this->AutoScaleDimensions = System::Drawing::SizeF(7, 15);
        this->ClientSize = System::Drawing::Size(1060, 552);
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
        this->Controls->Add(this->btnExport);
        this->Controls->Add(this->labelSearch);
        this->Controls->Add(this->textBoxSearch);
        this->Controls->Add(this->listViewExpenses);
        this->Controls->Add(this->labelIncome);
        this->Controls->Add(this->labelExpense);
        this->Controls->Add(this->labelBalance);
        this->Controls->Add(this->labelStats);
        this->Controls->Add(this->labelBudgetAlert);
        this->Controls->Add(this->statusStrip);

        this->Text = L"Expense Tracker - Управление финансами";
        this->StartPosition = FormStartPosition::CenterScreen;
        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
        this->MaximizeBox = false;
        this->MinimumSize = System::Drawing::Size(1060, 590);

        this->alertTimer->Interval = 300000;
        this->alertTimer->Tick += gcnew EventHandler(this, &Form1::alertTimer_Tick);

        this->ResumeLayout(false);
        this->PerformLayout();
    }

    void Form1::RefreshTransactionList() {
        listViewExpenses->Items->Clear();
        auto& transactions = manager->getTransactions();
        int count = 0;

        for (auto& t : transactions) {
            if (currentFilterCategory != L"Все") {
                String^ catStr = gcnew String(manager->categoryToDisplayName(t.getCategory()).c_str());
                if (catStr != currentFilterCategory) continue;
            }
            if (currentFilterType != L"Все") {
                String^ typeStr = gcnew String(manager->typeToDisplayName(t.getType()).c_str());
                if (typeStr != currentFilterType) continue;
            }
            if (!String::IsNullOrWhiteSpace(currentSearchTerm)) {
                String^ descStr = gcnew String(t.getDescription().c_str());
                if (descStr->ToLower()->IndexOf(currentSearchTerm->ToLower()) == -1) continue;
            }

            ListViewItem^ item = gcnew ListViewItem(t.getId().ToString());
            item->SubItems->Add(gcnew String(t.getDescription().c_str()));
            item->SubItems->Add(FormatAmount(t.getAmount()));
            item->SubItems->Add(gcnew String(manager->categoryToDisplayName(t.getCategory()).c_str()));
            item->SubItems->Add(gcnew String(manager->typeToDisplayName(t.getType()).c_str()));
            item->SubItems->Add(FormatDate(t.getDate()));

            if (t.getType() == TransactionType::INCOME) {
                item->ForeColor = Color::FromArgb(76, 175, 80);
            }
            else {
                item->ForeColor = Color::FromArgb(244, 67, 54);
            }

            listViewExpenses->Items->Add(item);
            count++;
        }

        UpdateStatusBar();
    }

    void Form1::UpdateTotals() {
        double totalIncome = manager->getTotalByType(TransactionType::INCOME);
        double totalExpense = manager->getTotalByType(TransactionType::EXPENSE);
        double balance = totalIncome - totalExpense;

        labelIncome->Text = String::Format(L"Доходы: {0:F2} ₽", totalIncome);
        labelExpense->Text = String::Format(L"Расходы: {0:F2} ₽", totalExpense);
        labelBalance->Text = String::Format(L"Баланс: {0:F2} ₽", balance);

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

            if (percent >= 100) {
                labelBudgetAlert->ForeColor = Color::FromArgb(244, 67, 54);
                labelBudgetAlert->Text += L"\nБЮДЖЕТ ПРЕВЫШЕН!";
            }
            else if (percent >= 90) {
                labelBudgetAlert->ForeColor = Color::FromArgb(255, 152, 0);
                labelBudgetAlert->Text += L"\nБюджет почти исчерпан!";
            }
            else if (percent >= 75) {
                labelBudgetAlert->ForeColor = Color::FromArgb(255, 193, 7);
            }
            else {
                labelBudgetAlert->ForeColor = Color::FromArgb(76, 175, 80);
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
            UpdateTotals();
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
            UpdateTotals();
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
            UpdateTotals();
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
        currentSearchTerm = L"";
        comboBoxFilter->SelectedIndex = 0;
        comboBoxFilterType->SelectedIndex = 0;
        textBoxSearch->Clear();
        RefreshTransactionList();
        statusLabel->Text = L"Фильтры сброшены";
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
            statusLabel->Text = String::Format(L"Бюджет установлен: {0:F2} ₽", monthlyBudget);
        }
    }

    void Form1::btnQuickAdd_Click(Object^ sender, EventArgs^ e) {
        time_t now = time(nullptr);
        manager->addTransaction("Быстрый расход", 500.0, Category::FOOD, TransactionType::EXPENSE, now);
        RefreshTransactionList();
        UpdateTotals();
        statusLabel->Text = L"Быстрый расход добавлен";
    }

    void Form1::btnExport_Click(Object^ sender, EventArgs^ e) {
        SaveFileDialog^ saveDialog = gcnew SaveFileDialog();
        saveDialog->Filter = L"CSV files (*.csv)|*.csv";
        saveDialog->Title = L"Экспорт транзакций";
        saveDialog->FileName = L"transactions_export.csv";

        if (saveDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            try {
                cli::array<unsigned char>^ bom = gcnew cli::array<unsigned char>(3);
                bom[0] = 0xEF;
                bom[1] = 0xBB;
                bom[2] = 0xBF;

                System::IO::File::WriteAllBytes(saveDialog->FileName, bom);

                StreamWriter^ sw = gcnew StreamWriter(saveDialog->FileName, true, System::Text::Encoding::UTF8);
                sw->WriteLine(L"ID;Описание;Сумма;Категория;Тип;Дата");

                for each (ListViewItem ^ item in listViewExpenses->Items) {
                    sw->WriteLine(String::Format(L"{0};{1};{2};{3};{4};{5}",
                        item->Text,
                        item->SubItems[1]->Text,
                        item->SubItems[2]->Text,
                        item->SubItems[3]->Text,
                        item->SubItems[4]->Text,
                        item->SubItems[5]->Text));
                }

                sw->Close();
                statusLabel->Text = L"Данные экспортированы";
                MessageBox::Show(L"Экспорт завершён!", L"Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            catch (Exception^ ex) {
                MessageBox::Show(L"Ошибка при экспорте: " + ex->Message, L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }
    }

    void Form1::textBoxSearch_TextChanged(Object^ sender, EventArgs^ e) {
        currentSearchTerm = textBoxSearch->Text;
        RefreshTransactionList();
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
                    UpdateTotals();
                    statusLabel->Text = L"Запись изменена";
                }
                catch (...) {}
            }
        }
    }

    void Form1::ApplyTheme(bool dark) {
        Color backColor, foreColor, controlBack, listBack, buttonBack;
        if (dark) {
            backColor = Color::FromArgb(32, 32, 32);
            foreColor = Color::White;
            controlBack = Color::FromArgb(45, 45, 48);
            listBack = Color::FromArgb(45, 45, 48);
            buttonBack = Color::FromArgb(70, 70, 75);
        }
        else {
            backColor = SystemColors::Control;
            foreColor = SystemColors::ControlText;
            controlBack = SystemColors::Window;
            listBack = SystemColors::Window;
            buttonBack = SystemColors::Control;
        }
        this->BackColor = backColor;
        this->ForeColor = foreColor;

        for each (Control ^ c in this->Controls) {
            if (dynamic_cast<TextBox^>(c) || dynamic_cast<ComboBox^>(c) || dynamic_cast<DateTimePicker^>(c)) {
                c->BackColor = controlBack;
                c->ForeColor = foreColor;
            }
            if (dynamic_cast<Button^>(c)) {
                c->BackColor = buttonBack;
                c->ForeColor = foreColor;
            }
            if (dynamic_cast<Label^>(c)) {
                c->ForeColor = foreColor;
            }
        }
        listViewExpenses->BackColor = listBack;
        listViewExpenses->ForeColor = foreColor;
        statusStrip->BackColor = backColor;
        statusStrip->ForeColor = foreColor;
    }

    void Form1::alertTimer_Tick(Object^ sender, EventArgs^ e) {
        UpdateBudgetAlert();
    }

    void Form1::UpdateStatusBar() {
        int totalCount = manager->getTransactionCount();
        int visibleCount = listViewExpenses->Items->Count;
        statusLabel->Text = String::Format(L"Всего записей: {0} | Отображается: {1}", totalCount, visibleCount);
    }

    time_t Form1::DateTimeToTimeT(DateTime dt) {
        long long ticks = dt.Ticks - DateTime(1970, 1, 1).Ticks;
        return static_cast<time_t>(ticks / 10000000);
    }
}
