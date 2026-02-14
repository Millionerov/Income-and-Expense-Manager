#pragma once
#ifndef EXPENSE_H
#define EXPENSE_H

#include <string>
#include <ctime>

enum class Category { FOOD, TRANSPORT, ENTERTAINMENT, HEALTH, SHOPPING, OTHER };
enum class TransactionType { EXPENSE, INCOME };

class Expense {
private:
    int id;
    std::string description;
    double amount;
    Category category;
    TransactionType type;
    time_t date;
    time_t createdTime;

public:
    Expense(int id, const std::string& description, double amount, Category category, TransactionType type,
        time_t date = time(nullptr)) : id(id), description(description), amount(amount),
        category(category), type(type), date(date), createdTime(time(nullptr)) {
    }

    int getId() const { return id; }
    std::string getDescription() const { return description; }
    double getAmount() const { return amount; }
    Category getCategory() const { return category; }
    TransactionType getType() const { return type; }
    time_t getDate() const { return date; }
    time_t getCreatedTime() const { return createdTime; }

    void setDescription(const std::string& desc) { description = desc; }
    void setAmount(double amt) { amount = amt; }
    void setCategory(Category cat) { category = cat; }
    void setType(TransactionType t) { type = t; }
    void setDate(time_t d) { date = d; }
    void setCreatedTime(time_t t) { createdTime = t; }
};

#endif