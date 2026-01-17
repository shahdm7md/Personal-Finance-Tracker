#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <limits>

using namespace std;

bool isValidDouble(const string& str)
{
    if (str.empty()) return false;
    try
    {
        size_t pos;
        stod(str, &pos);
        return pos == str.length();
    }
    catch (...)
    {
        return false;
    }
}

bool isValidInt(const string& str)
{
    if (str.empty()) return false;
    try
    {
        size_t pos;
        stoi(str, &pos);
        return pos == str.length();
    }
    catch (...)
    {
        return false;
    }
}

void clearInputBuffer()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

class Time
{
private:
    int hour;
    int minute;
    int second;

public:
    Time(int h = 0, int m = 0, int s = 0)
    {
        hour = (h >= 0 && h <= 23) ? h : 0;
        minute = (m >= 0 && m <= 59) ? m : 0;
        second = (s >= 0 && s <= 59) ? s : 0;
    }

    int getHour() const
    {
        return hour;
    }

    int getMinute() const
    {
        return minute;
    }

    int getSecond() const
    {
        return second;
    }

    string toString() const
    {
        char buf[10];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hour, minute, second);
        return buf;
    }
};

class Date
{
private:
    int year;
    int month;
    int day;
    Time time;

public:
    Date(int y = 2000, int m = 1, int d = 1, Time t = Time()) : time(t)
    {
        year = (y >= 1900 && y <= 9999) ? y : 2000;
        month = (m >= 1 && m <= 12) ? m : 1;
        int maxDay = 31;
        if (m == 2) maxDay = 28;
        else if (m == 4 || m == 6 || m == 9 || m == 11) maxDay = 30;
        day = (d >= 1 && d <= maxDay) ? d : 1;
    }

    int getYear() const
    {
        return year;
    }

    int getMonth() const
    {
        return month;
    }

    int getDay() const
    {
        return day;
    }

    Time getTime() const
    {
        return time;
    }

    string toString() const
    {
        char buf[20];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
                 year, month, day, time.getHour(), time.getMinute(), time.getSecond());
        return buf;
    }

    bool operator<(const Date& o) const
    {
        if (year != o.year) return year < o.year;
        if (month != o.month) return month < o.month;
        if (day != o.day) return day < o.day;
        if (time.getHour() != o.time.getHour()) return time.getHour() < o.time.getHour();
        if (time.getMinute() != o.time.getMinute()) return time.getMinute() < o.time.getMinute();
        return time.getSecond() < o.time.getSecond();
    }

    bool operator>(const Date& other) const
    {
        return other < *this;
    }

    bool operator<=(const Date& other) const
    {
        return !(*this > other);
    }

    bool operator>=(const Date& other) const
    {
        return !(*this < other);
    }
};

Date getCurrentDate()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return Date(
               1900 + ltm->tm_year,
               1 + ltm->tm_mon,
               ltm->tm_mday,
               Time(ltm->tm_hour, ltm->tm_min, ltm->tm_sec)
           );
}

class Transaction
{
private:
    double amount;
    Date date;

public:
    Transaction(double amn, Date dt) : amount(amn), date(dt) {}

    virtual ~Transaction() {}

    double getAmount() const
    {
        return amount;
    }

    Date getDate() const
    {
        return date;
    }

    void setAmount(double amt)
    {
        amount = amt;
    }

    void setDate(Date dt)
    {
        date = dt;
    }

    virtual void viewTrans() const = 0;
};

class Income : public Transaction
{
private:
    string source;

public:
    Income(string src, double amt, Date dt) : Transaction(amt, dt), source(src) {}

    void setSource(string src)
    {
        source = src;
    }

    string getSource() const
    {
        return source;
    }

    void viewTrans() const override
    {
        cout << "Source: " << getSource() << " ,Amount: " << getAmount() << " ,Date: " << getDate().toString() << endl;
    }
};

class Expense : public Transaction
{
private:
    string category;

public:
    Expense(string cat, double amt, Date dt) : Transaction(amt, dt), category(cat) {}

    void setCategory(string cat)
    {
        category = cat;
    }

    string getCategory() const
    {
        return category;
    }

    void viewTrans() const override
    {
        cout << "Category: " << getCategory() << " ,Amount: " << getAmount() << " ,Date: " << getDate().toString() << endl;
    }
};

class Budget
{
private:
    string category;
    double amount;

public:
    Budget(string cat, double amt) : category(cat), amount(amt) {}

    void setCategory(string cat)
    {
        category = cat;
    }

    string getCategory() const
    {
        return category;
    }

    double getAmount() const
    {
        return amount;
    }

    void setAmount(double amt)
    {
        amount = amt;
    }
};

class FinanceTracker
{
private:
    vector<Income> incomes;
    vector<Expense> expenses;
    vector<Budget> budgets;

    double calculateTotalIncome() const
    {
        double totalIncome = 0;
        for (const auto& inc : incomes) totalIncome += inc.getAmount();
        return totalIncome;
    }

    double calculateTotalExpenses() const
    {
        double totalExpenses = 0;
        for (const auto& exp : expenses) totalExpenses += exp.getAmount();
        return totalExpenses;
    }

    double calculateSpentInCategory(string category) const
    {
        double totalExpenses = 0;
        for (const auto& exp : expenses)
        {
            if (exp.getCategory() == category) totalExpenses += exp.getAmount();
        }
        return totalExpenses;
    }

    double getBudgetOfCategory(string category) const
    {
        for (const auto& b : budgets)
        {
            if (b.getCategory() == category) return b.getAmount();
        }
        return 0;
    }

public:
    FinanceTracker()
    {
        loadFromFile("finance_data.csv");
    }

    ~FinanceTracker()
    {
        saveToFile("finance_data.csv");
    }

    void addIncome(double amt, Date dt, string src)
    {
        if (amt < 0)
        {
            cout << "Error: Negative amount!\n";
            return;
        }
        if (src.empty())
        {
            cout << "Error: Source cannot be empty!\n";
            return;
        }
        if (amt > 10000) cout << "Alert: Large income added (" << amt << ")!\n";
        incomes.push_back(Income(src, amt, dt));
        cout << "Income added successfully!\n";
    }

    void viewIncomes() const
    {
        if (incomes.empty())
        {
            cout << "No incomes.\n";
            return;
        }
        cout << "\n=== All Incomes ===\n";
        for (size_t i = 0; i < incomes.size(); ++i)
        {
            cout << i << ": ";
            incomes[i].viewTrans();
        }
    }

    void updateIncome(int index, double amt, Date dt, string src)
    {
        if (index < 0 || index >= static_cast<int>(incomes.size()))
        {
            cout << "Invalid index!\n";
            return;
        }
        if (amt < 0)
        {
            cout << "Error: Negative amount!\n";
            return;
        }
        if (src.empty())
        {
            cout << "Error: Source cannot be empty!\n";
            return;
        }
        incomes[index].setAmount(amt);
        incomes[index].setDate(dt);
        incomes[index].setSource(src);
        cout << "Income updated successfully!\n";
    }

    void updateIncome(int index, int updateChoice, double amt, string src, Date dt)
    {
        if (index < 0 || index >= static_cast<int>(incomes.size()))
        {
            cout << "Invalid index!\n";
            return;
        }

        if (updateChoice == 1)
        {
            if (amt < 0)
            {
                cout << "Error: Negative amount!\n";
                return;
            }
            incomes[index].setAmount(amt);
        }
        else if (updateChoice == 2)
        {
            if (src.empty())
            {
                cout << "Error: Source cannot be empty!\n";
                return;
            }
            incomes[index].setSource(src);
        }
        else if (updateChoice == 3)
        {
            incomes[index].setDate(dt);
        }
        else if (updateChoice == 4)
        {
            if (amt < 0)
            {
                cout << "Error: Negative amount!\n";
                return;
            }
            if (src.empty())
            {
                cout << "Error: Source cannot be empty!\n";
                return;
            }
            incomes[index].setAmount(amt);
            incomes[index].setSource(src);
            incomes[index].setDate(dt);
        }
        cout << "Income updated successfully!\n";
    }

    void deleteIncome(int index)
    {
        if (index < 0 || index >= static_cast<int>(incomes.size()))
        {
            cout << "Invalid index!\n";
            return;
        }
        incomes.erase(incomes.begin() + index);
        cout << "Income deleted successfully!\n";
    }

    void addExpense(double amt, Date dt, string cat)
    {
        if (amt < 0)
        {
            cout << "Error: Negative amount!\n";
            return;
        }
        if (cat.empty())
        {
            cout << "Error: Category cannot be empty!\n";
            return;
        }

        double currentBalance = calculateTotalIncome() - calculateTotalExpenses();
        if (amt > currentBalance)
        {
            cout << "\n*** Warning: The current balance ("<< currentBalance
                 << ") is not enough for this expense ("<< amt << ") ***\n";

            cout << "Do you still want to continue? (y/n): ";

            char confirm;

            cin >> confirm;

            clearInputBuffer();

            if (confirm != 'y' && confirm != 'Y')

            {
                cout << "Expense canceled.\n";

                return;

            }
        }

        if (amt > 10000) cout << "Alert: Large expense added (" << amt << ")!\n";
        expenses.push_back(Expense(cat, amt, dt));
        cout << "Expense added successfully!\n";
        checkBudgets();
    }

    void viewExpenses() const
    {
        if (expenses.empty())
        {
            cout << "No expenses.\n";
            return;
        }
        cout << "\n=== All Expenses ===\n";
        for (size_t i = 0; i < expenses.size(); ++i)
        {
            cout << i << ": ";
            expenses[i].viewTrans();
        }
    }

    void updateExpense(int index, double amt, Date dt, string cat)
    {
        if (index < 0 || index >= static_cast<int>(expenses.size()))
        {
            cout << "Invalid index!\n";
            return;
        }
        if (amt < 0)
        {
            cout << "Error: Negative amount!\n";
            return;
        }
        if (cat.empty())
        {
            cout << "Error: Category cannot be empty!\n";
            return;
        }
        expenses[index].setAmount(amt);
        expenses[index].setDate(dt);
        expenses[index].setCategory(cat);
        cout << "Expense updated successfully!\n";
    }

    void updateExpense(int index, int updateChoice, double amt, string cat, Date dt)
    {
        if (index < 0 || index >= static_cast<int>(expenses.size()))
        {
            cout << "Invalid index!\n";
            return;
        }

        if (updateChoice == 1)
        {
            if (amt < 0)
            {
                cout << "Error: Negative amount!\n";
                return;
            }
            expenses[index].setAmount(amt);
        }
        else if (updateChoice == 2)
        {
            if (cat.empty())
            {
                cout << "Error: Category cannot be empty!\n";
                return;
            }
            expenses[index].setCategory(cat);
        }
        else if (updateChoice == 3)
        {
            expenses[index].setDate(dt);
        }
        else if (updateChoice == 4)
        {
            if (amt < 0)
            {
                cout << "Error: Negative amount!\n";
                return;
            }
            if (cat.empty())
            {
                cout << "Error: Category cannot be empty!\n";
                return;
            }
            expenses[index].setAmount(amt);
            expenses[index].setCategory(cat);
            expenses[index].setDate(dt);
        }
        cout << "Expense updated successfully!\n";
    }

    void deleteExpense(int index)
    {
        if (index < 0 || index >= static_cast<int>(expenses.size()))
        {
            cout << "Invalid index!\n";
            return;
        }
        expenses.erase(expenses.begin() + index);
        cout << "Expense deleted successfully!\n";
    }

    void setBudget(string category, double limit)
    {
        if (limit < 0)
        {
            cout << "Error: Negative limit!\n";
            return;
        }
        if (category.empty())
        {
            cout << "Error: Category cannot be empty!\n";
            return;
        }
        for (auto& budget : budgets)
        {
            if (budget.getCategory() == category)
            {
                budget.setAmount(limit);
                cout << "Budget for " << category << " updated to " << limit << endl;
                return;
            }
        }
        budgets.push_back(Budget(category, limit));
        cout << "New budget created for " << category << ": " << limit << endl;
    }

    void checkBudgets() const
    {
        if (budgets.empty())
        {
            cout << "\nNo budgets have been set yet.\n";
            return;
        }
        cout << "\nBudget Status:\n";
        for (const auto& b : budgets)
        {
            string cat = b.getCategory();
            double limit = b.getAmount();
            double spent = calculateSpentInCategory(cat);
            double percentage = (limit > 0) ? (spent / limit) * 100 : 0;
            cout << cat << ": ";
            if (spent > limit)
            {
                cout << "EXCEEDED by " << (spent - limit) << " (" << fixed << setprecision(1) << percentage << "%)\n";
            }
            else if (spent > limit * 0.9)
            {
                cout << "WARNING: " << fixed << setprecision(1) << percentage << "% used (" << (limit - spent) << " remaining)\n";
            }
            else
            {
                cout << fixed << setprecision(1) << percentage << "% used (" << (limit - spent) << " remaining)\n";
            }
        }
        cout << endl;
    }

    void generateSummary() const
    {
        double income = calculateTotalIncome();
        double expense = calculateTotalExpenses();
        double balance = income - expense;
        cout << "\n=== Financial Summary ===\n";
        cout << "Total Income: " << fixed << setprecision(2) << income << endl;
        cout << "Total Expenses: " << fixed << setprecision(2) << expense << endl;
        cout << "Current Balance: " << fixed << setprecision(2) << balance << endl;
        cout << "========================\n\n";
    }

    void generateDetailed(string category) const
    {
        if (category.empty())
        {
            cout << "Error: Category cannot be empty!\n";
            return;
        }
        generateSummary();
        cout << "Your Budget for category : " << category << " is " << getBudgetOfCategory(category) << endl
             << "You spent " << calculateSpentInCategory(category) << " on this category" << endl
             << "Details:" << endl;
        bool found = false;
        for (size_t i = 0; i < expenses.size(); ++i)
        {
            if (expenses[i].getCategory() == category)
            {
                expenses[i].viewTrans();
                found = true;
            }
        }
        if (!found) cout << "No expenses found in this category.\n";
    }

    void generateReportForPeriod(Date start, Date end) const
    {
        if (start > end)
        {
            cout << "Error: The start date is newer than the end date! We will be swapping the dates.\n";
            Date temp = start;
            start = end;
            end = temp;
        }
        cout << "\nTransactions from " << start.toString() << " to " << end.toString() << ":\n";
        cout << "\nYour Incomes:\n";
        bool foundIncome = false;
        for (size_t i = 0; i < incomes.size(); ++i)
        {
            if (incomes[i].getDate() >= start && incomes[i].getDate() <= end)
            {
                incomes[i].viewTrans();
                foundIncome = true;
            }
        }
        if (!foundIncome) cout << "No incomes in this period.\n";

        cout << "\nYour Expenses:\n";
        bool foundExpense = false;
        for (size_t i = 0; i < expenses.size(); ++i)
        {
            if (expenses[i].getDate() >= start && expenses[i].getDate() <= end)
            {
                expenses[i].viewTrans();
                foundExpense = true;
            }
        }
        if (!foundExpense) cout << "No expenses in this period.\n";
    }

    void saveToFile(string filename) const
    {
        ofstream file(filename);
        if (!file.is_open())
        {
            cout << "Error opening file for saving!\n";
            return;
        }
        for (const auto& inc : incomes)
        {
            file << "INCOME," << inc.getSource() << "," << inc.getAmount() << "," << inc.getDate().toString() << "\n";
        }
        for (const auto& exp : expenses)
        {
            file << "EXPENSE," << exp.getCategory() << "," << exp.getAmount() << "," << exp.getDate().toString() << "\n";
        }
        for (const auto& b : budgets)
        {
            file << "BUDGET," << b.getCategory() << "," << b.getAmount() << "\n";
        }
        file.close();
        cout << "Data saved to " << filename << endl;
    }

    void loadFromFile(string filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cout << "No save file found—starting fresh.\n";
            return;
        }

        string line;
        int lineNum = 0;
        while (getline(file, line))
        {
            lineNum++;
            if (line.empty()) continue;

            stringstream ss(line);
            string type, field1, field2, field3;

            getline(ss, type, ',');
            getline(ss, field1, ',');
            getline(ss, field2, ',');
            getline(ss, field3);

            if (type.empty() || field1.empty()) continue;

            if (type == "BUDGET")
            {
                // Format: BUDGET,category,amount
                try
                {
                    double amt = stod(field2);
                    budgets.push_back(Budget(field1, amt));
                }
                catch (...)
                {
                    cout << "Warning: Invalid budget on line " << lineNum << ", skipping.\n";
                }
            }
            else if (type == "INCOME" || type == "EXPENSE")
            {
                // Format: INCOME,source,amount,date
                // Format: EXPENSE,category,amount,date
                string category_or_source = field1;
                double amt;
                try
                {
                    amt = stod(field2);
                }
                catch (...)
                {
                    cout << "Warning: Invalid amount on line " << lineNum << ", skipping.\n";
                    continue;
                }

                // format is "YYYY-MM-DD HH:MM:SS"
                int y = 2000, m = 1, d = 1, h = 0, min = 0, sec = 0;
                sscanf(field3.c_str(), "%d-%d-%d %d:%d:%d", &y, &m, &d, &h, &min, &sec);
                Date dt(y, m, d, Time(h, min, sec));

                if (type == "INCOME")
                {
                    incomes.push_back(Income(category_or_source, amt, dt));
                }
                else if (type == "EXPENSE")
                {
                    expenses.push_back(Expense(category_or_source, amt, dt));
                }
            }
        }
        file.close();
        cout << "Loaded from " << filename << " (" << incomes.size() << " incomes, "
             << expenses.size() << " expenses, " << budgets.size() << " budgets)\n";
    }
};


int getValidatedInt(const string& prompt, int min = INT_MIN, int max = INT_MAX)
{
    string input;
    int value;
    while (true)
    {
        cout << prompt;
        getline(cin, input);
        if (isValidInt(input))
        {
            value = stoi(input);
            if (value >= min && value <= max)
                return value;
            else
                cout << "Please enter a number between " << min << " and " << max << ".\n";
        }
        else
        {
            cout << "Invalid input. Please enter a valid integer.\n";
        }
    }
}


double getValidatedDouble(const string& prompt, double min = 0)
{
    string input;
    double value;
    while (true)
    {
        cout << prompt;
        getline(cin, input);
        if (isValidDouble(input))
        {
            value = stod(input);
            if (value >= min)
                return value;
            else
                cout << "Please enter a number >= " << min << ".\n";
        }
        else
        {
            cout << "Invalid input. Please enter a valid number.\n";
        }
    }
}


string getValidatedString(const string& prompt)
{
    string input;
    while (true)
    {
        cout << prompt;
        getline(cin, input);
        if (!input.empty())
            return input;
        else
            cout << "Input cannot be empty. Please try again.\n";
    }
}

int main()
{
    FinanceTracker tracker;
    int choice;

    do
    {
        cout << "\n=== Personal Finance Tracker ===\n";
        cout << "1. Manage Incomes\n";
        cout << "2. Manage Expenses\n";
        cout << "3. Set Budget\n";
        cout << "4. Check Budget Status\n";
        cout << "5. Show Summary\n";
        cout << "6. Show Category Details\n";
        cout << "7. Show Transactions in Time Period\n";
        cout << "0. Exit & Save\n";

        choice = getValidatedInt("Choice: ", 0, 7);

        if (choice == 1)
        {
            int subChoice;
            cout << "\nIncome Management:\n";
            cout << "1. Add Income\n2. View Incomes\n3. Update Income\n4. Delete Income\n0. Back\n";
            subChoice = getValidatedInt("Choice: ", 0, 4);

            if (subChoice == 1)
            {
                double amt = getValidatedDouble("Amount: ", 0);
                string src = getValidatedString("Source: ");

                char ch;
                cout << "Do you want to enter date and time or add transaction with current date? (y/n): ";
                cin >> ch;
                clearInputBuffer();

                if (ch == 'y' || ch == 'Y')
                {
                    int y = getValidatedInt("Year (1900-9999): ", 1900, 9999);
                    int m = getValidatedInt("Month (1-12): ", 1, 12);
                    int d = getValidatedInt("Day (1-31): ", 1, 31);
                    int h = getValidatedInt("Hour (0-23): ", 0, 23);
                    int min = getValidatedInt("Minute (0-59): ", 0, 59);
                    int sec = getValidatedInt("Second (0-59): ", 0, 59);
                    tracker.addIncome(amt, Date(y, m, d, Time(h, min, sec)), src);
                }
                else
                {
                    tracker.addIncome(amt, getCurrentDate(), src);
                }
            }
            else if (subChoice == 2)
            {
                tracker.viewIncomes();
            }
            else if (subChoice == 3)
            {
                tracker.viewIncomes();
                int index = getValidatedInt("Index: ", 0);

                cout << "\nWhat do you want to update?\n";
                cout << "1. Amount\n";
                cout << "2. Source\n";
                cout << "3. Date\n";
                cout << "4. All\n";
                int updateChoice = getValidatedInt("Choice: ", 1, 4);

                double amt = 0;
                string src = "";
                Date dt = getCurrentDate();


                if (updateChoice == 1 || updateChoice == 4)
                {
                    amt = getValidatedDouble("New Amount: ", 0);
                }

                if (updateChoice == 2 || updateChoice == 4)
                {
                    src = getValidatedString("New Source: ");
                }

                if (updateChoice == 3 || updateChoice == 4)
                {
                    int y = getValidatedInt("New Year (1900-9999): ", 1900, 9999);
                    int m = getValidatedInt("New Month (1-12): ", 1, 12);
                    int d = getValidatedInt("New Day (1-31): ", 1, 31);
                    int h = getValidatedInt("New Hour (0-23): ", 0, 23);
                    int min = getValidatedInt("New Minute (0-59): ", 0, 59);
                    int sec = getValidatedInt("New Second (0-59): ", 0, 59);
                    dt = Date(y, m, d, Time(h, min, sec));
                }


                tracker.updateIncome(index, updateChoice, amt, src, dt);
            }
            else if (subChoice == 4)
            {
                tracker.viewIncomes();
                int index = getValidatedInt("Index: ", 0);
                tracker.deleteIncome(index);
            }
        }
        else if (choice == 2)
        {
            int subChoice;
            cout << "\nExpense Management:\n";
            cout << "1. Add Expense\n2. View Expenses\n3. Update Expense\n4. Delete Expense\n0. Back\n";
            subChoice = getValidatedInt("Choice: ", 0, 4);

            if (subChoice == 1)
            {
                double amt = getValidatedDouble("Amount: ", 0);
                string cat = getValidatedString("Category: ");
                  char ch;
                cout << "Do you want to enter date and time or add transaction with current date? (y/n): ";
                cin >> ch;
                clearInputBuffer();

                if (ch == 'y' || ch == 'Y')
                {
                    int y = getValidatedInt("Year (1900-9999): ", 1900, 9999);
                    int m = getValidatedInt("Month (1-12): ", 1, 12);
                    int d = getValidatedInt("Day (1-31): ", 1, 31);
                    int h = getValidatedInt("Hour (0-23): ", 0, 23);
                    int min = getValidatedInt("Minute (0-59): ", 0, 59);
                    int sec = getValidatedInt("Second (0-59): ", 0, 59);
                    tracker.addExpense(amt, Date(y, m, d, Time(h, min, sec)), cat);
                }
                else
                {
                    tracker.addExpense(amt, getCurrentDate(), cat);
                }

            }
            else if (subChoice == 2)
            {
                tracker.viewExpenses();
            }
            else if (subChoice == 3)
            {
                tracker.viewExpenses();
                int index = getValidatedInt("Index: ", 0);

                cout << "\nWhat do you want to update?\n";
                cout << "1. Amount\n";
                cout << "2. Category\n";
                cout << "3. Date\n";
                cout << "4. All\n";
                int updateChoice = getValidatedInt("Choice: ", 1, 4);

                double amt = 0;
                string cat = "";
                Date dt = getCurrentDate();

                if (updateChoice == 1 || updateChoice == 4)
                {
                    amt = getValidatedDouble("New Amount: ", 0);
                }

                if (updateChoice == 2 || updateChoice == 4)
                {
                    cat = getValidatedString("New Category: ");
                }

                if (updateChoice == 3 || updateChoice == 4)
                {
                    int y = getValidatedInt("New Year (1900-9999): ", 1900, 9999);
                    int m = getValidatedInt("New Month (1-12): ", 1, 12);
                    int d = getValidatedInt("New Day (1-31): ", 1, 31);
                    int h = getValidatedInt("New Hour (0-23): ", 0, 23);
                    int min = getValidatedInt("New Minute (0-59): ", 0, 59);
                    int sec = getValidatedInt("New Second (0-59): ", 0, 59);
                    dt = Date(y, m, d, Time(h, min, sec));
                }

                tracker.updateExpense(index, updateChoice, amt, cat, dt);
            }
            else if (subChoice == 4)
            {
                tracker.viewExpenses();
                int index = getValidatedInt("Index: ", 0);
                tracker.deleteExpense(index);
            }
        }
        else if (choice == 3)
        {
            string cat = getValidatedString("Category: ");
            double limit = getValidatedDouble("Limit: ", 0);
            tracker.setBudget(cat, limit);
        }
        else if (choice == 4)
        {
            tracker.checkBudgets();
        }
        else if (choice == 5)
        {
            tracker.generateSummary();
        }
        else if (choice == 6)
        {
            string cat = getValidatedString("Category: ");
            tracker.generateDetailed(cat);
        }
        else if (choice == 7)
        {
            cout << "\nStart Date:\n";
            int sy = getValidatedInt("Year (1900-9999): ", 1900, 9999);
            int sm = getValidatedInt("Month (1-12): ", 1, 12);
            int sd = getValidatedInt("Day (1-31): ", 1, 31);

            cout << "\nEnd Date:\n";
            int ey = getValidatedInt("Year (1900-9999): ", 1900, 9999);
            int em = getValidatedInt("Month (1-12): ", 1, 12);
            int ed = getValidatedInt("Day (1-31): ", 1, 31);

            tracker.generateReportForPeriod(Date(sy, sm, sd, Time(0, 0, 0)),
                                            Date(ey, em, ed, Time(23, 59, 59)));
        }
    }
    while (choice != 0);

    cout << "\nThank you for using Personal Finance Tracker!\n";
    return 0;
}
