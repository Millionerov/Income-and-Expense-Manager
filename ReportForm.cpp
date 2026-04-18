#include "ExpenseManager.h"
#include "ReportForm.h"
#include <vector>
#include <fstream>
#include <ctime>
#include <iomanip>

#using <System.IO.dll>

using namespace System::IO;
using namespace System::Text;

namespace ExpenseTrackerApp {
    ReportForm::ReportForm(ExpenseManager* mgr) {
        manager = mgr;

        form = gcnew Form();
        form->Text = L"Отчёт по расходам и доходам";
        form->Width = 600;
        form->Height = 550;
        form->BackColor = Color::FromArgb(32, 32, 32);
        form->ForeColor = Color::White;
        form->StartPosition = FormStartPosition::CenterParent;
        form->FormBorderStyle = FormBorderStyle::FixedDialog;
        form->MaximizeBox = false;
        form->MinimizeBox = false;

        labelTitle = gcnew Label();
        labelTitle->Text = L"Детальный отчёт по категориям";
        labelTitle->Location = Point(10, 10);
        labelTitle->Size = System::Drawing::Size(400, 35);
        labelTitle->Font = gcnew Drawing::Font(L"Segoe UI", 12, FontStyle::Bold);
        labelTitle->ForeColor = Color::White;

        labelTotalInfo = gcnew Label();
        labelTotalInfo->Location = Point(10, 45);
        labelTotalInfo->Size = System::Drawing::Size(550, 25);
        labelTotalInfo->Font = gcnew Drawing::Font(L"Segoe UI", 9);
        labelTotalInfo->ForeColor = Color::Gray;

        listViewReport = gcnew ListView();
        listViewReport->Location = Point(10, 80);
        listViewReport->Size = System::Drawing::Size(560, 380);
        listViewReport->View = View::Details;
        listViewReport->FullRowSelect = true;
        listViewReport->GridLines = true;
        listViewReport->Columns->Add(L"Категория", 150, HorizontalAlignment::Left);
        listViewReport->Columns->Add(L"Доходы (₽)", 130, HorizontalAlignment::Right);
        listViewReport->Columns->Add(L"Расходы (₽)", 130, HorizontalAlignment::Right);
        listViewReport->Columns->Add(L"Разница (₽)", 130, HorizontalAlignment::Right);

        btnClose = gcnew Button();
        btnClose->Text = L"Закрыть";
        btnClose->Location = Point(240, 475);
        btnClose->Size = System::Drawing::Size(100, 35);
        btnClose->FlatStyle = FlatStyle::Flat;
        btnClose->BackColor = Color::FromArgb(70, 70, 75);
        btnClose->ForeColor = Color::White;
        btnClose->Click += gcnew EventHandler(this, &ReportForm::btnClose_Click);

        btnExportReport = gcnew Button();
        btnExportReport->Text = L"Экспорт";
        btnExportReport->Location = Point(360, 475);
        btnExportReport->Size = System::Drawing::Size(100, 35);
        btnExportReport->FlatStyle = FlatStyle::Flat;
        btnExportReport->BackColor = Color::FromArgb(70, 70, 75);
        btnExportReport->ForeColor = Color::White;
        btnExportReport->Click += gcnew EventHandler(this, &ReportForm::btnExportReport_Click);

        form->Controls->Add(labelTitle);
        form->Controls->Add(labelTotalInfo);
        form->Controls->Add(listViewReport);
        form->Controls->Add(btnClose);
        form->Controls->Add(btnExportReport);

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
            double diff = income - expense;

            ListViewItem^ item = gcnew ListViewItem(categories[i]);
            item->SubItems->Add(income > 0 ? String::Format(L"{0:F2}", income) : L"0.00");
            item->SubItems->Add(expense > 0 ? String::Format(L"{0:F2}", expense) : L"0.00");
            item->SubItems->Add(String::Format(L"{0:F2}", diff));

            if (diff < 0) item->ForeColor = Color::FromArgb(244, 67, 54);
            else if (diff > 0) item->ForeColor = Color::FromArgb(76, 175, 80);

            this->listViewReport->Items->Add(item);
        }

        ListViewItem^ totalItem = gcnew ListViewItem(L"ИТОГО");
        totalItem->SubItems->Add(String::Format(L"{0:F2}", totalIncomeAll));
        totalItem->SubItems->Add(String::Format(L"{0:F2}", totalExpenseAll));
        totalItem->SubItems->Add(String::Format(L"{0:F2}", totalIncomeAll - totalExpenseAll));
        totalItem->Font = gcnew Drawing::Font(L"Segoe UI", 9, FontStyle::Bold);
        totalItem->ForeColor = Color::FromArgb(76, 175, 80);
        this->listViewReport->Items->Add(totalItem);

        double balance = totalIncomeAll - totalExpenseAll;
        labelTotalInfo->Text = String::Format(L"Общий доход: {0:F2} ₽ | Общий расход: {1:F2} ₽ | Баланс: {2:F2} ₽",
            totalIncomeAll, totalExpenseAll, balance);
    }

    void ReportForm::btnClose_Click(Object^ sender, EventArgs^ e) {
        form->Close();
    }

    void ReportForm::btnExportReport_Click(Object^ sender, EventArgs^ e) {
        SaveFileDialog^ saveDialog = gcnew SaveFileDialog();
        saveDialog->Filter = L"CSV files (*.csv)|*.csv";
        saveDialog->Title = L"Экспорт отчёта";
        saveDialog->FileName = L"report_export.csv";

        if (saveDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            try {
                System::Text::Encoding^ win1251 = System::Text::Encoding::GetEncoding(1251);
                StreamWriter^ sw = gcnew StreamWriter(saveDialog->FileName, false, win1251);

                sw->WriteLine(L"Категория;Доходы (₽);Расходы (₽);Разница (₽)");

                for each (ListViewItem ^ item in listViewReport->Items) {
                    sw->WriteLine(String::Format(L"{0};{1};{2};{3}",
                        item->Text,
                        item->SubItems[1]->Text,
                        item->SubItems[2]->Text,
                        item->SubItems[3]->Text));
                }

                sw->Close();
                MessageBox::Show(L"Отчёт экспортирован успешно!", L"Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            catch (Exception^ ex) {
                MessageBox::Show(L"Ошибка при экспорте: " + ex->Message, L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }
    }
}

