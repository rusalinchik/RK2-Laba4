#include <iostream>
#include <string>

class BankAccount {
protected:
    int accountNumber;
    std::string ownerName;
    double balance;

public:
    BankAccount(int accNumber, std::string owner, double bal) {
        accountNumber = accNumber;
        ownerName = owner;
        balance = bal;
    }

    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            std::cout << "Счет пополнен на " << amount << std::endl;
        }
    }

    void withdraw(double amount) {
        if (amount <= balance) {
            balance -= amount;
            std::cout << "Со счета снято " << amount << std::endl;
        }
        else {
            std::cout << "Ошибка: недостаточно средств!" << std::endl;
        }
    }

    void display() {
        std::cout << "Номер счета: " << accountNumber << std::endl;
        std::cout << "Владелец: " << ownerName << std::endl;
        std::cout << "Баланс: " << balance << std::endl;
    }
};

class SavingsAccount : public BankAccount {
private:
    double interestRate;

public:
    SavingsAccount(int accNumber, std::string owner, double bal, double rate)
        : BankAccount(accNumber, owner, bal) {
        interestRate = rate;
    }

    void addInterest() {
        double interest = balance * interestRate / 100;
        balance += interest;
        std::cout << "Начислены проценты: " << interest << std::endl;
    }
};

int main() {
    setlocale(LC_ALL, "RU");
    BankAccount account1(101, "Мяу Мяукович", 1000);
    account1.deposit(500);
    account1.withdraw(300);
    account1.display();

    std::cout << "------------------------" << std::endl;

    SavingsAccount account2(202, "Акакий Акакиевич", 2000, 5);
    account2.deposit(1000);
    account2.addInterest();
    account2.display();

    return 0;
}
