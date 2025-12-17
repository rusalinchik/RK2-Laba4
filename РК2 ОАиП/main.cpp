#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <limits>

// =======================
// Базовый класс FoodItem
// =======================
class FoodItem {
protected:
    std::string name;
    std::string expirationDate;
    double weight;
    std::vector<std::string> suppliers;

public:
    FoodItem(const std::string& n,
        const std::string& exp,
        double w)
        : name(n), expirationDate(exp), weight(w) {
    }

    virtual ~FoodItem() = default;

    virtual void display() const {
        std::cout << "Name: " << name
            << ", Expiration: " << expirationDate
            << ", Weight: " << weight << "\n";

        std::cout << "Suppliers: ";
        for (const auto& s : suppliers)
            std::cout << s << " ";
        std::cout << "\n";
    }

    void addSupplier(const std::string& s) {
        suppliers.push_back(s);
    }

    const std::string& getExpirationDate() const { return expirationDate; }
    double getWeight() const { return weight; }
    void edit(double newWeight, const std::string& newDate) {
        weight = newWeight;
        expirationDate = newDate;
    }
};

class Vegetable : public FoodItem {
    std::string type;
    int calories;

public:
    Vegetable(const std::string& n,
        const std::string& exp,
        double w,
        const std::string& t,
        int cal)
        : FoodItem(n, exp, w), type(t), calories(cal) {
    }

    int getCalories() const { return calories; }

    void display() const override {
        std::cout << "[Vegetable]\n";
        FoodItem::display();
        std::cout << "Type: " << type
            << ", Calories: " << calories << "\n\n";
    }
};

class DairyProduct : public FoodItem {
    double fat;
    int protein;

public:
    DairyProduct(const std::string& n,
        const std::string& exp,
        double w,
        double f,
        int p)
        : FoodItem(n, exp, w), fat(f), protein(p) {
    }

    void display() const override {
        std::cout << "[Dairy]\n";
        FoodItem::display();
        std::cout << "Fat: " << fat
            << "%, Protein: " << protein << "g\n\n";
    }
};

// ===== Функции =====
void loadFromFile(const std::string& filename,
    std::vector<std::unique_ptr<FoodItem>>& items)
{
    std::ifstream file(filename);
    if (!file) { std::cout << "Файл не найден\n"; return; }

    std::string line;
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string type, name, date, weightStr, param, suppliersStr;

        getline(ss, type, ';');
        getline(ss, name, ';');
        getline(ss, date, ';');
        getline(ss, weightStr, ';');
        getline(ss, param, ';');
        getline(ss, suppliersStr);

        double weight = std::stod(weightStr);
        std::unique_ptr<FoodItem> item;

        if (type == "Vegetable") {
            auto pos = param.find('-');
            item = std::make_unique<Vegetable>(
                name, date, weight,
                param.substr(0, pos),
                std::stoi(param.substr(pos + 1)));
        }
        else {
            auto pos = param.find('-');
            item = std::make_unique<DairyProduct>(
                name, date, weight,
                std::stod(param.substr(0, pos)),
                std::stoi(param.substr(pos + 9)));
        }

        std::stringstream supSS(suppliersStr);
        std::string sup;
        while (getline(supSS, sup, '|'))
            item->addSupplier(sup);

        items.push_back(std::move(item));
    }
}

void removeExpired(std::vector<std::unique_ptr<FoodItem>>& items,
    const std::string& today)
{
    items.erase(
        std::remove_if(items.begin(), items.end(),
            [&](const auto& f) { return f->getExpirationDate() < today; }),
        items.end());
}

void findMaxCaloriesVegetable(
    const std::vector<std::unique_ptr<FoodItem>>& items)
{
    auto it = std::max_element(
        items.begin(), items.end(),
        [](const auto& a, const auto& b) {
            auto va = dynamic_cast<Vegetable*>(a.get());
            auto vb = dynamic_cast<Vegetable*>(b.get());
            if (!va) return true;
            if (!vb) return false;
            return va->getCalories() < vb->getCalories();
        });

    if (it != items.end()) (*it)->display();
}

void editProduct(std::vector<std::unique_ptr<FoodItem>>& items) {
    if (items.empty()) { std::cout << "Список продуктов пуст.\n"; return; }

    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << i + 1 << ". "; items[i]->display();
    }

    size_t choice; std::cout << "Выберите номер продукта: "; std::cin >> choice;
    if (choice < 1 || choice > items.size()) { std::cout << "Неверный номер.\n"; return; }

    FoodItem* product = items[choice - 1].get();
    double newWeight; std::string newDate, newSupplier;

    std::cout << "Введите новый вес: "; std::cin >> newWeight;
    std::cout << "Введите новую дату годности (YYYY-MM-DD): "; std::cin >> newDate;
    product->edit(newWeight, newDate);

    std::cout << "Введите нового поставщика: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, newSupplier);
    product->addSupplier(newSupplier);

    std::cout << "Продукт обновлён.\n";
}

void printTotalWeightByType(const std::vector<std::unique_ptr<FoodItem>>& items) {
    double totalVegetables = std::accumulate(items.begin(), items.end(), 0.0,
        [](double sum, const std::unique_ptr<FoodItem>& f) {
            return sum + (dynamic_cast<Vegetable*>(f.get()) ? f->getWeight() : 0.0);
        });
    double totalDairy = std::accumulate(items.begin(), items.end(), 0.0,
        [](double sum, const std::unique_ptr<FoodItem>& f) {
            return sum + (dynamic_cast<DairyProduct*>(f.get()) ? f->getWeight() : 0.0);
        });

    std::cout << "Суммарный вес овощей: " << totalVegetables << "\n";
    std::cout << "Суммарный вес молочных продуктов: " << totalDairy << "\n";
}

int main() {
    setlocale(LC_ALL, "RU");
    std::vector<std::unique_ptr<FoodItem>> warehouse;
    loadFromFile("warehouse.txt", warehouse);

    int choice;
    do {
        std::cout << "\n===== Меню =====\n";
        std::cout << "1. Показать все продукты\n";
        std::cout << "2. Удалить просроченные продукты\n";
        std::cout << "3. Найти овощ с максимальным количеством калорий\n";
        std::cout << "4. Редактировать продукт\n";
        std::cout << "5. Показать суммарный вес по типу\n";
        std::cout << "0. Выход\n";
        std::cout << "Выберите действие: "; std::cin >> choice;

        switch (choice) {
        case 1: for (auto& f : warehouse) f->display(); break;
        case 2: {
            std::string today; std::cout << "Введите сегодняшнюю дату (YYYY-MM-DD): "; std::cin >> today;
            removeExpired(warehouse, today);
            std::cout << "Просроченные продукты удалены.\n"; break;
        }
        case 3: findMaxCaloriesVegetable(warehouse); break;
        case 4: editProduct(warehouse); break;
        case 5: printTotalWeightByType(warehouse); break;
        case 0: break;
        default: std::cout << "Неверный выбор.\n"; break;
        }

    } while (choice != 0);

    std::cout << "Выход из программы.\n";
    return 0;
}