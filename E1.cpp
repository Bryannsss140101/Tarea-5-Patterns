#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

class Account {
private:
    std::string id;
    double balance;
    mutable std::mutex mtx;

public:
    Account(std::string id, double initial_balance)
        : id(std::move(id)), balance(initial_balance) {}

    void deposit(double amount) {
        std::lock_guard<std::mutex> lock(mtx);
        balance += amount;
    }

    void withdraw(double amount) {
        std::lock_guard<std::mutex> lock(mtx);
        balance -= amount;
    }

    double getBalance() const {
        std::lock_guard<std::mutex> lock(mtx);
        return balance;
    }
};

class TransactionManager {
private:
    TransactionManager() = default;
    ~TransactionManager() = default;
    TransactionManager(const TransactionManager&) = delete;
    TransactionManager& operator=(const TransactionManager&) = delete;

public:
    static TransactionManager& getInstance() {
        static TransactionManager instance;
        return instance;
    }

    void deposit(Account* account, double amount) {
        if (account) {
            account->deposit(amount);
        }
    }

    void withdraw(Account* account, double amount) {
        if (account) {
            account->withdraw(amount);
        }
    }
};

int main() {
    auto& manager = TransactionManager::getInstance();
    Account account("A1", 1000);
    std::vector<std::thread> threads;

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            manager.deposit(&account, 100);
            manager.withdraw(&account, 50);
        });
    }

    for (auto& t : threads) t.join();

    std::cout << "Balance final: " << account.getBalance() << std::endl;
    std::cout << "Dirección manager: " << &TransactionManager::getInstance() << std::endl;

    return 0;
}