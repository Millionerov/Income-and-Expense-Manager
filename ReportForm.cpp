#include "ExpenseManager.h"
#include "ReportForm.h"
#include <vector>

namespace ExpenseTrackerApp {
    ReportForm::ReportForm(ExpenseManager* mgr) {
        manager = mgr;

        form = gcnew Form();
        form->Text = L"Отчет по расходам и доходам";
        form->Width = 500;
        form->Height = 500;
        form->BackColor = Color::FromArgb(32, 32, 32);
        form->ForeColor = Color::White;
        form->StartPosition = FormStartPosition::CenterParent;
        form->FormBorderStyle = FormBorderStyle::FixedDialog;
        form->MaximizeBox = false;
        form->MinimizeBox = false;

        labelTitle = gcnew Label();
        labelTitle->Text = L"Детальный отчет по категориям";
        labelTitle->Location = Point(10, 10);
        labelTitle->Size = System::Drawing::Size(400, 30);
        labelTitle->Font = gcnew System::Drawing::Font(L"Segoe UI", 12, FontStyle::Bold);
        labelTitle->ForeColor = Color::White;

        listViewReport = gcnew ListView();
        listViewReport->Location = Point(10, 50);
        listViewReport->Size = System::Drawing::Size(460, 350);
        listViewReport->View = View::Details;
        listViewReport->FullRowSelect = true;
        listViewReport->GridLines = true;
        listViewReport->Columns->Add(L"Категория", 150, HorizontalAlignment::Left);
        listViewReport->Columns->Add(L"Доходы (₽)", 130, HorizontalAlignment::Right);
        listViewReport->Columns->Add(L"Расходы (₽)", 130, HorizontalAlignment::Right);

        btnClose = gcnew Button();
        btnClose->Text = L"Закрыть";
        btnClose->Location = Point(190, 410);
        btnClose->Size = System::Drawing::Size(100, 35);
        btnClose->FlatStyle = FlatStyle::Flat;
        btnClose->BackColor = Color::FromArgb(70, 70, 75);
        btnClose->ForeColor = Color::White;
        btnClose->Click += gcnew EventHandler(this, &ReportForm::btnClose_Click);

        form->Controls->Add(labelTitle);
        form->Controls->Add(listViewReport);
        form->Controls->Add(btnClose);

        LoadReport();
    }

    ReportForm::~ReportForm() {
        delete form;
    }

    DialogResult ReportForm::ShowDialog() {
        return form->ShowDialog();
    }

    void ReportForm::Close() {
        form->Close();
    }

    void ReportForm::LoadReport() {
        if (this->listViewReport == nullptr) return;
        this->listViewReport->Items->Clear();

        cli::array<String^>^ categories = gcnew cli::array<String^> { L"Еда", L"Транспорт", L"Развлечения", L"Здоровье", L"Покупки", L"Другое" };

        cli::array<Category>^ catEnums = gcnew cli::array<Category> {
            Category::FOOD, Category::TRANSPORT, Category::ENTERTAINMENT,
                Category::HEALTH, Category::SHOPPING, Category::OTHER
        };

        double totalIncomeAll = 0;
        double totalExpenseAll = 0;

        const std::vector<Expense>& allTransactions = manager->getTransactions();

        for (int i = 0; i < categories->Length; i++) {
            double income = 0;
            double expense = 0;
            Category currentCat = catEnums[i];

            for (size_t j = 0; j < allTransactions.size(); j++) {
                if (allTransactions[j].getCategory() == currentCat) {
                    if (allTransactions[j].getType() == TransactionType::INCOME) {
                        income += allTransactions[j].getAmount();
                    }
                    else {
                        expense += allTransactions[j].getAmount();
                    }
                }
            }

            totalIncomeAll += income;
            totalExpenseAll += expense;

            ListViewItem^ item = gcnew ListViewItem(categories[i]);
            item->SubItems->Add(income > 0 ? String::Format(L"{0:F2}", income) : L"0.00");
            item->SubItems->Add(expense > 0 ? String::Format(L"{0:F2}", expense) : L"0.00");

            if (expense > income) item->ForeColor = Color::FromArgb(244, 67, 54);
            else if (income > expense) item->ForeColor = Color::FromArgb(76, 175, 80);

            this->listViewReport->Items->Add(item);
        }

        ListViewItem^ totalItem = gcnew ListViewItem(L"ИТОГО");
        totalItem->SubItems->Add(String::Format(L"{0:F2}", totalIncomeAll));
        totalItem->SubItems->Add(String::Format(L"{0:F2}", totalExpenseAll));
        totalItem->Font = gcnew System::Drawing::Font(L"Segoe UI", 9, FontStyle::Bold);
        totalItem->ForeColor = Color::FromArgb(76, 175, 80); 
        this->listViewReport->Items->Add(totalItem);

        double balance = totalIncomeAll - totalExpenseAll;
        ListViewItem^ balanceItem = gcnew ListViewItem(L"БАЛАНС");
        balanceItem->SubItems->Add(L"");
        balanceItem->SubItems->Add(String::Format(L"{0:F2}", balance));
        balanceItem->Font = gcnew System::Drawing::Font(L"Segoe UI", 9, FontStyle::Bold);
        balanceItem->ForeColor = (balance >= 0) ? Color::FromArgb(76, 175, 80) : Color::FromArgb(244, 67, 54);
        this->listViewReport->Items->Add(balanceItem);
    }

    void ReportForm::btnClose_Click(Object^ sender, EventArgs^ e) {
        form->Close();
    }
}