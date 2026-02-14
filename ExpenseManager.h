#pragma once
#ifndef EXPENSE_MANAGER_H
#define EXPENSE_MANAGER_H

#define _CRT_SECURE_NO_WARNINGS

#include "Expense.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

class ExpenseManager {
private:
    vector<Expense> transactions;
    int next_id = 1;
    const string filename = "transactions.txt";

    Category stringToCategory(const string& str) {
        if (str == "FOOD") return Category::FOOD;
        if (str == "TRANSPORT") return Category::TRANSPORT;
        if (str == "ENTERTAINMENT") return Category::ENTERTAINMENT;
        if (str == "HEALTH") return Category::HEALTH;
        if (str == "SHOPPING") return Category::SHOPPING;
        return Category::OTHER;
    }

    TransactionType stringToType(const string& str) {
        if (str == "INCOME") return TransactionType::INCOME;
        return TransactionType::EXPENSE;
    }

    string categoryToString(Category cat) {
        switch (cat) {
        case Category::FOOD: return "FOOD";
        case Category::TRANSPORT: return "TRANSPORT";
        case Category::ENTERTAINMENT: return "ENTERTAINMENT";
        case Category::HEALTH: return "HEALTH";
        case Category::SHOPPING: return "SHOPPING";
        case Category::OTHER: return "OTHER";
        default: return "OTHER";
        }
    }

    string typeToString(TransactionType type) {
        return type == TransactionType::INCOME ? "INCOME" : "EXPENSE";
    }

    void loadFromFile() {
        ifstream file(filename);
        if (!file.is_open()) return;

        transactions.clear();
        string line;
        int max_id = 0;

        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;

            while (getline(ss, token, '|')) {
                tokens.push_back(token);
            }

            if (tokens.size() >= 7) {
                int id = stoi(tokens[0]);
                string description = tokens[1];
                double amount = stod(tokens[2]);
                Category category = stringToCategory(tokens[3]);
                TransactionType type = stringToType(tokens[4]);
                time_t date = stol(tokens[5]);
                time_t createdTime = stol(tokens[6]);

                Expense transaction(id, description, amount, category, type, date);
                transaction.setCreatedTime(createdTime);
                transactions.push_back(transaction);

                if (id > max_id) max_id = id;
            }
        }
        next_id = max_id + 1;
        file.close();
    }

public:
    ExpenseManager() { loadFromFile(); }
    ~ExpenseManager() { saveToFile(); }

    string categoryToDisplayName(Category cat) {
        switch (cat) {
        case Category::FOOD: return "Еда";
        case Category::TRANSPORT: return "Транспорт";
        case Category::ENTERTAINMENT: return "Развлечения";
        case Category::HEALTH: return "Здоровье";
        case Category::SHOPPING: return "Покупки";
        case Category::OTHER: return "Другое";
        default: return "Другое";
        }
    }

    string typeToDisplayName(TransactionType type) {
        return type == TransactionType::INCOME ? "Доход" : "Расход";
    }

    void addTransaction(const string& description, double amount, Category category, TransactionType type, time_t date) {
        if (description.empty() || amount <= 0) return;
        transactions.emplace_back(next_id, description, amount, category, type, date);
        next_id++;
        saveToFile();
    }

    vector<Expense>& getTransactions() { return transactions; }

    void deleteTransaction(int id) {
        auto it = find_if(transactions.begin(), transactions.end(),
            [id](const Expense& t) { return t.getId() == id; });
        if (it != transactions.end()) {
            transactions.erase(it);
            saveToFile();

            if (transactions.empty()) {
                next_id = 1;
            }
            else {
                int max_id = 0;
                for (const auto& t : transactions) {
                    if (t.getId() > max_id) {
                        max_id = t.getId();
                    }
                }
                next_id = max_id + 1;
            }
        }
    }

    void updateTransaction(int id, const string& description, double amount, Category category, TransactionType type, time_t date) {
        auto it = find_if(transactions.begin(), transactions.end(),
            [id](const Expense& t) { return t.getId() == id; });
        if (it != transactions.end()) {
            it->setDescription(description);
            it->setAmount(amount);
            it->setCategory(category);
            it->setType(type);
            it->setDate(date);
            saveToFile();
        }
    }

    double getTotalByType(TransactionType type) {
        double total = 0;
        for (const auto& t : transactions) {
            if (t.getType() == type) {
                total += t.getAmount();
            }
        }
        return total;
    }

    double getTotalByCategoryAndType(Category category, TransactionType type) {
        double total = 0;
        for (const auto& t : transactions) {
            if (t.getCategory() == category && t.getType() == type) {
                total += t.getAmount();
            }
        }
        return total;
    }

    void saveToFile() {
        ofstream file(filename);
        if (!file.is_open()) return;
        for (const auto& t : transactions) {
            file << t.getId() << "|" << t.getDescription() << "|" << t.getAmount() << "|"
                << categoryToString(t.getCategory()) << "|" << typeToString(t.getType()) << "|"
                << t.getDate() << "|" << t.getCreatedTime() << endl;
        }
        file.close();
    }
};

#endif