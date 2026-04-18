#include "StatisticsForm.h"
#include "ExpenseManager.h"
#include <ctime>
#include <map>

#using <System.IO.dll>

using namespace System::IO;
using namespace System::Text;

namespace ExpenseTrackerApp {
    StatisticsForm::StatisticsForm(ExpenseManager* mgr) {
        manager = mgr;

        this->Text = L"📊 Статистика расходов и доходов";
        this->MinimumSize = System::Drawing::Size(800, 600);
        this->Size = System::Drawing::Size(1000, 700);
        this->StartPosition = FormStartPosition::CenterParent;
        this->BackColor = Color::FromArgb(32, 32, 32);

        panelTop = gcnew Panel();
        panelTop->Location = Point(0, 0);
        panelTop->Size = System::Drawing::Size(1000, 50);
        panelTop->BackColor = Color::FromArgb(45, 45, 48);
        panelTop->Dock = DockStyle::Top;

        panelBottom = gcnew Panel();
        panelBottom->Location = Point(0, 650);
        panelBottom->Size = System::Drawing::Size(1000, 50);
        panelBottom->BackColor = Color::FromArgb(45, 45, 48);
        panelBottom->Dock = DockStyle::Bottom;

        tabControl = gcnew TabControl();
        tabControl->Dock = DockStyle::Fill;
        tabControl->BackColor = Color::FromArgb(32, 32, 32);

        TabPage^ tabPie = gcnew TabPage();
        tabPie->Text = L"🥧 Круговая диаграмма";
        tabPie->BackColor = Color::FromArgb(32, 32, 32);

        TabPage^ tabBar = gcnew TabPage();
        tabBar->Text = L"📊 Столбчатая диаграмма";
        tabBar->BackColor = Color::FromArgb(32, 32, 32);

        TabPage^ tabLine = gcnew TabPage();
        tabLine->Text = L"📈 График по месяцам";
        tabLine->BackColor = Color::FromArgb(32, 32, 32);

        tabControl->TabPages->Add(tabPie);
        tabControl->TabPages->Add(tabBar);
        tabControl->TabPages->Add(tabLine);

        chartPie = gcnew Chart();
        chartPie->Dock = DockStyle::Fill;
        chartPie->BackColor = Color::FromArgb(45, 45, 48);

        ChartArea^ pieArea = gcnew ChartArea();
        pieArea->BackColor = Color::FromArgb(45, 45, 48);
        chartPie->ChartAreas->Add(pieArea);

        chartBar = gcnew Chart();
        chartBar->Dock = DockStyle::Fill;
        chartBar->BackColor = Color::FromArgb(45, 45, 48);

        ChartArea^ barArea = gcnew ChartArea();
        barArea->AxisX->Title = L"Категория";
        barArea->AxisY->Title = L"Сумма (₽)";
        barArea->AxisX->TitleForeColor = Color::White;
        barArea->AxisY->TitleForeColor = Color::White;
        barArea->AxisX->LabelStyle->ForeColor = Color::White;
        barArea->AxisY->LabelStyle->ForeColor = Color::White;
        barArea->BackColor = Color::FromArgb(45, 45, 48);
        chartBar->ChartAreas->Add(barArea);

        chartLine = gcnew Chart();
        chartLine->Dock = DockStyle::Fill;
        chartLine->BackColor = Color::FromArgb(45, 45, 48);

        ChartArea^ lineArea = gcnew ChartArea();
        lineArea->AxisX->Title = L"Месяц";
        lineArea->AxisY->Title = L"Сумма (₽)";
        lineArea->AxisX->TitleForeColor = Color::White;
        lineArea->AxisY->TitleForeColor = Color::White;
        lineArea->AxisX->LabelStyle->ForeColor = Color::White;
        lineArea->AxisY->LabelStyle->ForeColor = Color::White;
        lineArea->BackColor = Color::FromArgb(45, 45, 48);
        chartLine->ChartAreas->Add(lineArea);

        tabPie->Controls->Add(chartPie);
        tabBar->Controls->Add(chartBar);
        tabLine->Controls->Add(chartLine);

        labelTotalInfo = gcnew Label();
        labelTotalInfo->Location = Point(15, 10);
        labelTotalInfo->Size = System::Drawing::Size(500, 30);
        labelTotalInfo->ForeColor = Color::Gray;
        labelTotalInfo->Font = gcnew Drawing::Font(L"Segoe UI", 9);
        labelTotalInfo->Text = L"Загрузка данных...";

        Label^ labelYear = gcnew Label();
        labelYear->Text = L"Год:";
        labelYear->Location = Point(530, 12);
        labelYear->Size = System::Drawing::Size(40, 25);
        labelYear->ForeColor = Color::White;

        comboBoxYear = gcnew ComboBox();
        comboBoxYear->Location = Point(570, 10);
        comboBoxYear->Size = System::Drawing::Size(80, 25);
        comboBoxYear->DropDownStyle = ComboBoxStyle::DropDownList;
        comboBoxYear->BackColor = Color::FromArgb(45, 45, 48);
        comboBoxYear->ForeColor = Color::White;

        Label^ labelMonth = gcnew Label();
        labelMonth->Text = L"Месяц:";
        labelMonth->Location = Point(660, 12);
        labelMonth->Size = System::Drawing::Size(50, 25);
        labelMonth->ForeColor = Color::White;

        comboBoxMonth = gcnew ComboBox();
        comboBoxMonth->Location = Point(710, 10);
        comboBoxMonth->Size = System::Drawing::Size(100, 25);
        comboBoxMonth->DropDownStyle = ComboBoxStyle::DropDownList;
        comboBoxMonth->BackColor = Color::FromArgb(45, 45, 48);
        comboBoxMonth->ForeColor = Color::White;

        btnExportStats = gcnew Button();
        btnExportStats->Text = L"📁 Экспорт статистики";
        btnExportStats->Location = Point(830, 8);
        btnExportStats->Size = System::Drawing::Size(150, 30);
        btnExportStats->FlatStyle = FlatStyle::Flat;
        btnExportStats->BackColor = Color::FromArgb(70, 70, 75);
        btnExportStats->ForeColor = Color::White;
        btnExportStats->Click += gcnew EventHandler(this, &StatisticsForm::btnExportStats_Click);

        btnClose = gcnew Button();
        btnClose->Text = L"✖ Закрыть";
        btnClose->Location = Point(880, 8);
        btnClose->Size = System::Drawing::Size(100, 30);
        btnClose->FlatStyle = FlatStyle::Flat;
        btnClose->BackColor = Color::FromArgb(70, 70, 75);
        btnClose->ForeColor = Color::White;
        btnClose->Click += gcnew EventHandler(this, &StatisticsForm::btnClose_Click);

        panelTop->Controls->Add(labelTotalInfo);
        panelTop->Controls->Add(labelYear);
        panelTop->Controls->Add(comboBoxYear);
        panelTop->Controls->Add(labelMonth);
        panelTop->Controls->Add(comboBoxMonth);
        panelTop->Controls->Add(btnExportStats);
        panelTop->Controls->Add(btnClose);

        time_t now = time(nullptr);
        struct tm tm_now;
        localtime_s(&tm_now, &now);
        int currentYear = tm_now.tm_year + 1900;

        for (int y = currentYear - 2; y <= currentYear + 1; y++) {
            comboBoxYear->Items->Add(y.ToString());
        }
        comboBoxYear->SelectedIndex = 2;

        comboBoxMonth->Items->Add(L"Январь");
        comboBoxMonth->Items->Add(L"Февраль");
        comboBoxMonth->Items->Add(L"Март");
        comboBoxMonth->Items->Add(L"Апрель");
        comboBoxMonth->Items->Add(L"Май");
        comboBoxMonth->Items->Add(L"Июнь");
        comboBoxMonth->Items->Add(L"Июль");
        comboBoxMonth->Items->Add(L"Август");
        comboBoxMonth->Items->Add(L"Сентябрь");
        comboBoxMonth->Items->Add(L"Октябрь");
        comboBoxMonth->Items->Add(L"Ноябрь");
        comboBoxMonth->Items->Add(L"Декабрь");
        comboBoxMonth->SelectedIndex = tm_now.tm_mon;

        comboBoxYear->SelectedIndexChanged += gcnew EventHandler(this, &StatisticsForm::comboBoxYear_SelectedIndexChanged);
        comboBoxMonth->SelectedIndexChanged += gcnew EventHandler(this, &StatisticsForm::comboBoxMonth_SelectedIndexChanged);

        this->Controls->Add(tabControl);
        this->Controls->Add(panelTop);
        this->Controls->Add(panelBottom);

        LoadPieChart();
        LoadBarChart();
        LoadLineChart();
        LoadMonthStats();
    }

    StatisticsForm::~StatisticsForm() {
        delete chartPie;
        delete chartBar;
        delete chartLine;
    }

    void StatisticsForm::LoadPieChart() {
        chartPie->Series->Clear();

        std::map<Category, double> expensesByCategory;

        expensesByCategory[Category::FOOD] = 0;
        expensesByCategory[Category::TRANSPORT] = 0;
        expensesByCategory[Category::ENTERTAINMENT] = 0;
        expensesByCategory[Category::HEALTH] = 0;
        expensesByCategory[Category::SHOPPING] = 0;
        expensesByCategory[Category::OTHER] = 0;

        for (auto& t : manager->getTransactions()) {
            if (t.getType() == TransactionType::EXPENSE) {
                expensesByCategory[t.getCategory()] += t.getAmount();
            }
        }

        Series^ pieSeries = gcnew Series();
        pieSeries->Name = L"Расходы по категориям";
        pieSeries->ChartType = SeriesChartType::Pie;
        pieSeries->IsValueShownAsLabel = true;
        pieSeries->LabelFormat = L"{0:F0}%";
        pieSeries->Font = gcnew Drawing::Font(L"Segoe UI", 8);

        if (expensesByCategory[Category::FOOD] > 0)
            pieSeries->Points->AddXY(L"Еда", expensesByCategory[Category::FOOD]);
        if (expensesByCategory[Category::TRANSPORT] > 0)
            pieSeries->Points->AddXY(L"Транспорт", expensesByCategory[Category::TRANSPORT]);
        if (expensesByCategory[Category::ENTERTAINMENT] > 0)
            pieSeries->Points->AddXY(L"Развлечения", expensesByCategory[Category::ENTERTAINMENT]);
        if (expensesByCategory[Category::HEALTH] > 0)
            pieSeries->Points->AddXY(L"Здоровье", expensesByCategory[Category::HEALTH]);
        if (expensesByCategory[Category::SHOPPING] > 0)
            pieSeries->Points->AddXY(L"Покупки", expensesByCategory[Category::SHOPPING]);
        if (expensesByCategory[Category::OTHER] > 0)
            pieSeries->Points->AddXY(L"Другое", expensesByCategory[Category::OTHER]);

        if (pieSeries->Points->Count > 0) {
            chartPie->Series->Add(pieSeries);
        }
        else {
            pieSeries->Points->AddXY(L"Нет данных", 1);
            chartPie->Series->Add(pieSeries);
        }

        chartPie->Legends->Clear();
        Legend^ legend = gcnew Legend();
        legend->DockedToChartArea = pieSeries->ChartArea;
        legend->IsDockedInsideChartArea = false;
        legend->Docking = Docking::Right;
        legend->ForeColor = Color::White;
        chartPie->Legends->Add(legend);

        chartPie->Titles->Clear();
        Title^ title = gcnew Title(L"Распределение расходов по категориям", Docking::Top,
            gcnew Drawing::Font(L"Segoe UI", 12, FontStyle::Bold), Color::White);
        chartPie->Titles->Add(title);
    }

    void StatisticsForm::LoadBarChart() {
        chartBar->Series->Clear();

        int year = Int32::Parse(comboBoxYear->SelectedItem->ToString());
        int month = comboBoxMonth->SelectedIndex;

        Series^ expenseSeries = gcnew Series();
        expenseSeries->Name = L"Расходы";
        expenseSeries->ChartType = SeriesChartType::Column;
        expenseSeries->Color = Color::FromArgb(244, 67, 54);

        Series^ incomeSeries = gcnew Series();
        incomeSeries->Name = L"Доходы";
        incomeSeries->ChartType = SeriesChartType::Column;
        incomeSeries->Color = Color::FromArgb(76, 175, 80);

        double expenses[6] = { 0 };
        double incomes[6] = { 0 };

        for (auto& t : manager->getTransactions()) {
            struct tm tm_t;
            time_t t_date = t.getDate();
            localtime_s(&tm_t, &t_date);

            if (tm_t.tm_year + 1900 == year && tm_t.tm_mon == month) {
                double amount = t.getAmount();
                int index = 0;

                switch (t.getCategory()) {
                case Category::FOOD: index = 0; break;
                case Category::TRANSPORT: index = 1; break;
                case Category::ENTERTAINMENT: index = 2; break;
                case Category::HEALTH: index = 3; break;
                case Category::SHOPPING: index = 4; break;
                default: index = 5; break;
                }

                if (t.getType() == TransactionType::EXPENSE) {
                    expenses[index] += amount;
                }
                else {
                    incomes[index] += amount;
                }
            }
        }

        cli::array<String^>^ categories = { L"Еда", L"Транспорт", L"Развлечения", L"Здоровье", L"Покупки", L"Другое" };

        for (int i = 0; i < 6; i++) {
            expenseSeries->Points->AddXY(categories[i], expenses[i]);
            incomeSeries->Points->AddXY(categories[i], incomes[i]);
        }

        chartBar->Series->Add(expenseSeries);
        chartBar->Series->Add(incomeSeries);

        chartBar->Titles->Clear();
        Title^ title = gcnew Title(String::Format(L"Доходы и расходы за {0} {1}",
            comboBoxMonth->SelectedItem->ToString(), year), Docking::Top,
            gcnew Drawing::Font(L"Segoe UI", 12, FontStyle::Bold), Color::White);
        chartBar->Titles->Add(title);
    }

    void StatisticsForm::LoadLineChart() {
        chartLine->Series->Clear();

        int year = Int32::Parse(comboBoxYear->SelectedItem->ToString());

        cli::array<String^>^ months = { L"Янв", L"Фев", L"Мар", L"Апр", L"Май", L"Июн",
                                        L"Июл", L"Авг", L"Сен", L"Окт", L"Ноя", L"Дек" };

        Series^ expenseSeries = gcnew Series();
        expenseSeries->Name = L"Расходы";
        expenseSeries->ChartType = SeriesChartType::Line;
        expenseSeries->Color = Color::FromArgb(244, 67, 54);
        expenseSeries->BorderWidth = 3;
        expenseSeries->MarkerStyle = MarkerStyle::Circle;
        expenseSeries->MarkerSize = 8;

        Series^ incomeSeries = gcnew Series();
        incomeSeries->Name = L"Доходы";
        incomeSeries->ChartType = SeriesChartType::Line;
        incomeSeries->Color = Color::FromArgb(76, 175, 80);
        incomeSeries->BorderWidth = 3;
        incomeSeries->MarkerStyle = MarkerStyle::Circle;
        incomeSeries->MarkerSize = 8;

        for (int m = 0; m < 12; m++) {
            double expense = 0;
            double income = 0;

            for (auto& t : manager->getTransactions()) {
                struct tm tm_t;
                time_t t_date = t.getDate();
                localtime_s(&tm_t, &t_date);

                if (tm_t.tm_year + 1900 == year && tm_t.tm_mon == m) {
                    if (t.getType() == TransactionType::EXPENSE) {
                        expense += t.getAmount();
                    }
                    else {
                        income += t.getAmount();
                    }
                }
            }

            expenseSeries->Points->AddXY(months[m], expense);
            incomeSeries->Points->AddXY(months[m], income);
        }

        chartLine->Series->Add(expenseSeries);
        chartLine->Series->Add(incomeSeries);

        chartLine->Titles->Clear();
        Title^ title = gcnew Title(String::Format(L"Динамика доходов и расходов за {0} год", year), Docking::Top,
            gcnew Drawing::Font(L"Segoe UI", 12, FontStyle::Bold), Color::White);
        chartLine->Titles->Add(title);
    }

    void StatisticsForm::LoadMonthStats() {
        int year = Int32::Parse(comboBoxYear->SelectedItem->ToString());
        int month = comboBoxMonth->SelectedIndex;

        double totalIncome = 0;
        double totalExpense = 0;

        for (auto& t : manager->getTransactions()) {
            struct tm tm_t;
            time_t t_date = t.getDate();
            localtime_s(&tm_t, &t_date);

            if (tm_t.tm_year + 1900 == year && tm_t.tm_mon == month) {
                if (t.getType() == TransactionType::INCOME) {
                    totalIncome += t.getAmount();
                }
                else {
                    totalExpense += t.getAmount();
                }
            }
        }

        double balance = totalIncome - totalExpense;

        labelTotalInfo->Text = String::Format(L"💰 Доходы: {0:F2} ₽ | 💸 Расходы: {1:F2} ₽ | ⚖️ Баланс: {2:F2} ₽",
            totalIncome, totalExpense, balance);
    }

    void StatisticsForm::comboBoxYear_SelectedIndexChanged(Object^ sender, EventArgs^ e) {
        LoadBarChart();
        LoadLineChart();
        LoadMonthStats();
    }

    void StatisticsForm::comboBoxMonth_SelectedIndexChanged(Object^ sender, EventArgs^ e) {
        LoadBarChart();
        LoadMonthStats();
    }

    void StatisticsForm::btnExportStats_Click(Object^ sender, EventArgs^ e) {
        SaveFileDialog^ saveDialog = gcnew SaveFileDialog();
        saveDialog->Filter = L"CSV files (*.csv)|*.csv";
        saveDialog->Title = L"Экспорт статистики";
        saveDialog->FileName = String::Format(L"statistics_{0}.csv", DateTime::Now.ToString(L"yyyyMMdd"));

        if (saveDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            try {
                System::Text::Encoding^ win1251 = System::Text::Encoding::GetEncoding(1251);
                StreamWriter^ sw = gcnew StreamWriter(saveDialog->FileName, false, win1251);

                sw->WriteLine(L"=== СТАТИСТИКА РАСХОДОВ И ДОХОДОВ ===");
                sw->WriteLine(L"Дата выгрузки;{0}", DateTime::Now.ToString());
                sw->WriteLine(L"");

                sw->WriteLine(L"=== РАСХОДЫ ПО КАТЕГОРИЯМ ===");
                sw->WriteLine(L"Категория;Сумма (₽)");

                double categories[6] = { 0 };

                for (auto& t : manager->getTransactions()) {
                    if (t.getType() == TransactionType::EXPENSE) {
                        switch (t.getCategory()) {
                        case Category::FOOD: categories[0] += t.getAmount(); break;
                        case Category::TRANSPORT: categories[1] += t.getAmount(); break;
                        case Category::ENTERTAINMENT: categories[2] += t.getAmount(); break;
                        case Category::HEALTH: categories[3] += t.getAmount(); break;
                        case Category::SHOPPING: categories[4] += t.getAmount(); break;
                        default: categories[5] += t.getAmount(); break;
                        }
                    }
                }

                sw->WriteLine(L"Еда;{0:F2}", categories[0]);
                sw->WriteLine(L"Транспорт;{0:F2}", categories[1]);
                sw->WriteLine(L"Развлечения;{0:F2}", categories[2]);
                sw->WriteLine(L"Здоровье;{0:F2}", categories[3]);
                sw->WriteLine(L"Покупки;{0:F2}", categories[4]);
                sw->WriteLine(L"Другое;{0:F2}", categories[5]);

                sw->WriteLine(L"");
                sw->WriteLine(L"=== ОБЩАЯ СТАТИСТИКА ===");
                sw->WriteLine(L"Показатель;Сумма (₽)");
                sw->WriteLine(L"Общий доход;{0:F2}", manager->getTotalByType(TransactionType::INCOME));
                sw->WriteLine(L"Общий расход;{0:F2}", manager->getTotalByType(TransactionType::EXPENSE));
                sw->WriteLine(L"Баланс;{0:F2}", manager->getTotalByType(TransactionType::INCOME) - manager->getTotalByType(TransactionType::EXPENSE));
                sw->WriteLine(L"Количество транзакций;{0}", manager->getTransactionCount());

                sw->Close();
                MessageBox::Show(L"Статистика экспортирована успешно!", L"Успех",
                    MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            catch (Exception^ ex) {
                MessageBox::Show(L"Ошибка при экспорте: " + ex->Message, L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }
    }

    void StatisticsForm::btnClose_Click(Object^ sender, EventArgs^ e) {
        this->Close();
    }
}