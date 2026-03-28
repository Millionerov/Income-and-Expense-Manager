#pragma once

#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>
#using <System.Data.dll>

#include "ExpenseManager.h"
#include "EditForm.h"
#include <msclr/marshal_cppstd.h>
#include <ctime>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Collections::Generic;

namespace ExpenseTrackerApp {
    public ref class Form1 : public Form {
    private:
        ExpenseManager* manager;
        System::ComponentModel::Container^ components;
        TextBox^ textBoxDescription;
        TextBox^ textBoxAmount;
        ComboBox^ comboBoxCategory;
        ComboBox^ comboBoxType;
        ComboBox^ comboBoxFilter;
        ComboBox^ comboBoxFilterType;
        DateTimePicker^ dateTimePickerExpense;
        Button^ btnAdd;
        Button^ btnUpdate;
        Button^ btnDelete;
        Button^ btnFilter;
        Button^ btnTheme;
        ListView^ listViewExpenses;
        Label^ labelTotal;
        Label^ labelIncome;
        Label^ labelExpense;
        Label^ labelBalance;
        Label^ labelStats;
        StatusStrip^ statusStrip;
        ToolStripStatusLabel^ statusLabel;

        bool darkThemeEnabled;

        void InitializeComponent();
        void RefreshTransactionList();
        void UpdateTotals();
        String^ FormatDate(time_t time);
        String^ FormatAmount(double amount);
        void btnAdd_Click(Object^ sender, EventArgs^ e);
        void btnUpdate_Click(Object^ sender, EventArgs^ e);
        void btnDelete_Click(Object^ sender, EventArgs^ e);
        void btnFilter_Click(Object^ sender, EventArgs^ e);
        void btnTheme_Click(Object^ sender, EventArgs^ e);
        void comboBoxFilter_SelectedIndexChanged(Object^ sender, EventArgs^ e);
        void comboBoxFilterType_SelectedIndexChanged(Object^ sender, EventArgs^ e);
        void listViewExpenses_SelectedIndexChanged(Object^ sender, EventArgs^ e);
        void listViewExpenses_ColumnClick(Object^ sender, ColumnClickEventArgs^ e);
        void listViewExpenses_MouseDoubleClick(Object^ sender, MouseEventArgs^ e);
        void ApplyTheme(bool dark);
        time_t DateTimeToTimeT(DateTime dt);
        int selectedTransactionId;
        String^ currentFilterCategory;
        String^ currentFilterType;
        int lastSortedColumn;
        bool sortAscending;

    public:
        Form1();
        ~Form1();
    };
}
