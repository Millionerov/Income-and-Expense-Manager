#pragma once

#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.DataVisualization.dll>

#include "ExpenseManager.h"
#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Windows::Forms::DataVisualization::Charting;

namespace ExpenseTrackerApp {
    public ref class StatisticsForm : public Form {
    private:
        ExpenseManager* manager;
        TabControl^ tabControl;
        Chart^ chartPie;
        Chart^ chartBar;
        Chart^ chartLine;
        Button^ btnClose;
        Button^ btnExportStats;
        Label^ labelTotalInfo;
        ComboBox^ comboBoxYear;
        ComboBox^ comboBoxMonth;
        Panel^ panelTop;
        Panel^ panelBottom;

        void LoadPieChart();
        void LoadBarChart();
        void LoadLineChart();
        void LoadMonthStats();
        void btnClose_Click(Object^ sender, EventArgs^ e);
        void btnExportStats_Click(Object^ sender, EventArgs^ e);
        void comboBoxYear_SelectedIndexChanged(Object^ sender, EventArgs^ e);
        void comboBoxMonth_SelectedIndexChanged(Object^ sender, EventArgs^ e);

    public:
        StatisticsForm(ExpenseManager* mgr);
        ~StatisticsForm();
    };
}