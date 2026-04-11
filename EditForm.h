#pragma once

#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>

#include "Expense.h"
#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;

namespace ExpenseTrackerApp {
    public ref class EditForm : public Form {
    private:
        TextBox^ textBoxEditDescription;
        TextBox^ textBoxEditAmount;
        ComboBox^ comboBoxEditCategory;
        ComboBox^ comboBoxEditType;
        Button^ btnEditOK;
        Button^ btnEditCancel;
        Label^ labelDesc;
        Label^ labelAmount;
        Label^ labelCat;
        Label^ labelType;

        void InitializeComponent();
        void btnEditOK_Click(Object^ sender, EventArgs^ e);
        void btnEditCancel_Click(Object^ sender, EventArgs^ e);

    public:
        EditForm();
        String^ GetDescription() { return textBoxEditDescription->Text; }
        String^ GetAmount() { return textBoxEditAmount->Text; }
        Category GetCategory() { return (Category)comboBoxEditCategory->SelectedIndex; }
        TransactionType GetTransactionType() { return (TransactionType)comboBoxEditType->SelectedIndex; }
        void SetData(String^ desc, String^ amount, Category cat, TransactionType type);
    };
}